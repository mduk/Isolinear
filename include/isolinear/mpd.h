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

#include "mpd/playercontrolbar.h"


using std::cerr;


class View : public Drawable {
  protected:
    std::string title;
    Grid grid;

  public:
    View(std::string t, Grid g) : title{t}, grid{g} {}

    std::string Name() const {
      return title;
    }

    Region2D Bounds() const {
      return grid.bounds;
    }

};


class MPDView : public View {
  protected:
    mpdxx::Client& mpdc;
    Window& window;

  public:
    MPDView(std::string t, Grid g, Window& w, mpdxx::Client& _mpdc)
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
    NowPlayingView(Grid g, Window& w, mpdxx::Client& _mpdc)
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

      miso::connect(mpdc.signal_status, [this](mpdxx::StringMap status){
        hide = (status.at("state") == "stop");
      });

      miso::connect(mpdc.signal_current_song, [this](mpdxx::StringMap current_song){
        title.Right(current_song.at("Title"));
        album.Right(current_song.at("Album"));
        artist.Right(current_song.at("Artist"));
        duration.Right(current_song.at("duration"));
      });
    }

    void Draw(SDL_Renderer* renderer) const override {
      if (hide) {
        return;
      }
      MPDView::Draw(renderer);
    }

};


class QueueSongBar : public EastHeaderBar {
  protected:
    mpdxx::Song song;
    Button& playbtn;
    Button& deletebtn;

  public:
    QueueSongBar(Grid g, Window& w, mpdxx::Song s)
      : EastHeaderBar(g, w, Compass::EAST, s.Title())
      , song(s)
      , playbtn(AddButton("PLAY"))
      , deletebtn(AddButton("DELETE"))
    {
      deletebtn.Disable();

      RegisterChild(&playbtn);
      RegisterChild(&deletebtn);
    }
};


class QueueView : public MPDView {
  protected:
    mpdxx::SongList queue;
    std::list<QueueSongBar> songbars;

  public:
    QueueView(Grid g, Window& w, mpdxx::Client& _mpdc)
      : MPDView("QUEUE", g, w,  _mpdc)
    {}
};


class OutputsView : public MPDView {
  protected:
    mpdxx::OutputList outputs;
    std::list<EastHeaderBar> bars;

  public:
    OutputsView(Grid g, Window& w, mpdxx::Client& _mpdc)
      : MPDView("OUTPUTS", g, w, _mpdc)
    {}
};


class MpdFrame : public Drawable {
  protected:
    const std::string V_NOWPLAYING = "NOW PLAYING";
    const std::string V_QUEUE = "QUEUE";
    const std::string V_BROWSE = "BROWSE";
    const std::string V_ARTISTS = "ARTISTS";
    const std::string V_SEARCH = "SEARCH";
    const std::string V_OUTPUTS = "OUTPUTS";

    mpdxx::Client& mpdc;

    CompassLayout layout;

    EastHeaderBar hdrFrame;
    VerticalButtonBar barView;
    PlayerControlBar playerControlBar;
    NorthWestSweep sweepNorthWest;
    SouthWestSweep sweepSouthWest;

    std::map<const std::string, View*> views;
    std::string activeView{V_NOWPLAYING};

    NowPlayingView viewNowPlaying;
    QueueView viewQueue;
    OutputsView viewOutputs;


  public:
    MpdFrame(Grid g, Window& w, mpdxx::Client& _mpdc)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrFrame{layout.North(), w, Compass::EAST, " MPD CONTROL "}
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
        barView.DeactivateAll();
        auto active = miso::sender<Button>();
        active->Activate();
        activeView = active->Label();
        Update();
      };

      for (auto const& [view_name, view_ptr] : views) {
        Button& view_btn = barView.AddButton(view_name);
        miso::connect(view_btn.signal_press, switch_view);
      }

      barView.GetButton(activeView).Activate();

      playerControlBar.Update();
      Update();
    }

    void Update()
    {
      hdrFrame.Label(activeView + " : MPD");
      playerControlBar.Update();
      views.at(activeView)->Update();
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

