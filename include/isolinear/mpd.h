#pragma once

#include <list>
#include <map>
#include <string>
#include <iostream>

#include <fmt/core.h>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"
#include "progressbar.h"
#include "view.h"

#include "mpd/playercontrolbar.h"


using std::cerr;


class MPDView : public View {
  protected:
    mpdxx::client& mpdc;
    Window& window;

  public:
    MPDView(std::string t, Grid g, Window& w, mpdxx::client& _mpdc)
      : View(t, g)
      , window{w}
      , mpdc{_mpdc}
    {};
};


class NowPlayingView : public MPDView {
  protected:
    bool hide = false;
    PairHeaderBar title;
    PairHeaderBar album;
    PairHeaderBar artist;
    PairHeaderBar duration;
    HorizontalProgressBar progress;

  public:
    NowPlayingView(Grid g, Window& w, mpdxx::client& _mpdc)
      : MPDView("NOW PLAYING", g, w, _mpdc)
      , title(g.Rows(3,4), w, "TITLE", "[title]")
      , album(g.Rows(5,6), w, "ALBUM", "[album]")
      , artist(g.Rows(7,8), w, "ARTIST", "[artist]")
      , duration(g.Rows(9,10), w, "DURATION", "[duration]")
      , progress(g.Rows(11,12))
    {
      RegisterChild(&title);
      RegisterChild(&album);
      RegisterChild(&artist);
      RegisterChild(&duration);
      RegisterChild(&progress);

      miso::connect(mpdc.signal_status, [this](mpdxx::status status){
        hide = status.IsStopped();
      });

      miso::connect(mpdc.signal_current_song, [this](mpdxx::song song){
        title.Right(song.Title());
        album.Right(song.Album());
        artist.Right(song.Artist());
        duration.Right(song.DurationString());
      });
    }

    void Draw(SDL_Renderer* renderer) const override {
      if (hide) {
        return;
      }
      MPDView::Draw(renderer);
    }

};



class QueueView : public MPDView {
  protected:
    std::list<mpdxx::song> queue;

  public:
    QueueView(Grid g, Window& w, mpdxx::client& mpdc)
      : MPDView("QUEUE", g, w,  mpdc)
    {
      miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> newqueue){
        queue = newqueue;
      });
    }

    void Draw(SDL_Renderer* renderer) const override {
      int i = 1;
      for (auto const& song : queue) {
        EastHeaderBar songbar(
            grid.Rows((i*2)-1, i*2),
            window,
            song.Title()
          );
        songbar.Colours(Colours());
        songbar.Draw(renderer);
        i++;
      }
    }
};


class OutputsView : public MPDView {
  public:
    OutputsView(Grid g, Window& w, mpdxx::client& _mpdc)
      : MPDView("OUTPUTS", g, w, _mpdc)
    {}
};


class MpdFrame : public Drawable {
  public:
    const std::string V_NOWPLAYING = "NOW PLAYING";
    const std::string V_QUEUE = "QUEUE";
    const std::string V_BROWSE = "BROWSE";
    const std::string V_ARTISTS = "ARTISTS";
    const std::string V_SEARCH = "SEARCH";
    const std::string V_OUTPUTS = "OUTPUTS";

  public:
    miso::signal<std::string, std::string> signal_view_change;

  protected:
    mpdxx::client& mpdc;

    CompassLayout layout;

    EastHeaderBar hdrFrame;
    VerticalButtonBar barView;
    PlayerControlBar playerControlBar;
    NorthWestSweep sweepNorthWest;
    SouthWestSweep sweepSouthWest;

    std::map<const std::string, View*> views;
    std::string activeView = V_QUEUE;

    NowPlayingView viewNowPlaying;
    QueueView viewQueue;
    OutputsView viewOutputs;

  public:
    MpdFrame(Grid g, Window& w, mpdxx::client& _mpdc)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrFrame{layout.North(), w, Compass::EAST, "MPD CONTROL"}
        , barView{w, layout.West()}
        , playerControlBar{w, layout.South(), mpdc}
        , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
        , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

        , mpdc(_mpdc)

        , viewNowPlaying(layout.Centre(), w, mpdc)
        , viewQueue     (layout.Centre(), w, mpdc)
        , viewOutputs   (layout.Centre(), w, mpdc)
    {
      RegisterChild(&hdrFrame);
      RegisterChild(&barView);
      RegisterChild(&playerControlBar);
      RegisterChild(&sweepNorthWest);
      RegisterChild(&sweepSouthWest);

      RegisterView(&viewNowPlaying);
      RegisterView(&viewQueue);
      RegisterView(&viewOutputs);

      auto switch_view = [this]() {
        auto button = miso::sender<Button>();
        SwitchView(button->Label());
      };

      for (auto const& [view_name, view_ptr] : views) {
        Button& view_btn = barView.AddButton(view_name);
        miso::connect(view_btn.signal_press, switch_view);
      }

      barView.GetButton(activeView).Activate();
      emit signal_view_change("", activeView);

      playerControlBar.Update();
      Update();
    }

    virtual void SwitchView(std::string view) {
      if (view == activeView) {
        return;
      }

      auto previousView = activeView;
      activeView = view;

      barView.DeactivateAll();
      barView.ActivateOne(activeView);

      hdrFrame.Label(fmt::format("MPD : {}", activeView));

      emit signal_view_change(previousView, activeView);
    }

    virtual Region2D Bounds() const override {
      return layout.Bounds();
    }

    virtual void OnPointerEvent(PointerEvent event) {
      Drawable::OnPointerEvent(event);
      views.at(activeView)->OnPointerEvent(event);
    }

    virtual void Draw(SDL_Renderer* renderer) const {
      Drawable::Draw(renderer);
      views.at(activeView)->Draw(renderer);
    }

    virtual void Colours(ColourScheme cs) {
      Drawable::Colours(cs);
      for (auto const& [view_name, view_ptr] : views) {
        view_ptr->Colours(cs);
      }
    }

    void RegisterView(View* view) {
      const std::string view_name = view->Name();
      views.insert(std::pair<const std::string, View*>(view_name, view));
    }
};

