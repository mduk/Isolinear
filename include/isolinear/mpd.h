#pragma once

#include <list>
#include <map>
#include <string>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"

#include "mpd/playercontrolbar.h"


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
    MPDXX::Client& mpd;
    Window& window;

  public:
    MPDView(std::string t, Grid g, Window& w, MPDXX::Client& _mpd)
      : View(t, g)
      , window{w}
      , mpd{_mpd}
    {};
};


class NowPlayingView : public MPDView {
  protected:
    bool hide = false;
    PairHeaderBar title;
    PairHeaderBar album;
    PairHeaderBar artist;
    PairHeaderBar duration;

  public:
    NowPlayingView(Grid g, Window& w, MPDXX::Client& _mpd)
      : MPDView("NOW PLAYING", g, w, _mpd)
      , title(g.Rows(3,4), w, "TITLE", "[title]")
      , album(g.Rows(5,6), w, "ALBUM", "[album]")
      , artist(g.Rows(7,8), w, "ARTIST", "[artist]")
      , duration(g.Rows(9,10), w, "DURATION", "[duration]")
    {
      RegisterChild(&title);
      RegisterChild(&album);
      RegisterChild(&artist);
      RegisterChild(&duration);
    }

    void Update() override {
      if (mpd.IsPlaying() || mpd.IsPaused()) {
        MPDXX::Song now = mpd.CurrentlyPlaying();
        title.Right(now.Title());
        album.Right(now.Album());
        artist.Right(now.Artist());
        duration.Right(mpd.ElapsedTime() + " / " + now.Duration());
        hide = false;
      }
      else {
        hide = true;
      }
    }

    void Draw(SDL_Renderer* renderer) const override {
      if (hide) {
        return;
      }
      MPDView::Draw(renderer);
    }

};

class QueueView : public MPDView {
  public:
    QueueView(Grid g, Window& w, MPDXX::Client& _mpd)
      : MPDView("QUEUE", g, w, _mpd)
    {}

    void Draw(SDL_Renderer* renderer) const {
      int i = 1;
      for (auto const& song : mpd.Queue() ) {
        PairHeaderBar row{
            grid.Rows( i*2-1, i*2 ),
            window,
            song.Artist(),
            song.Title()
          };
        row.Colours(Colours());
        row.Draw(renderer);
        i++;

        if (i > 11) {
          break;
        }
      }
    }
};


class OutputsView : public MPDView {
  protected:
    std::list<EastHeaderBar> bars;

  public:
    OutputsView(Grid g, Window& w, MPDXX::Client& _mpd)
      : MPDView("OUTPUTS", g, w, _mpd)
    {
      int i = 1;
      for (auto const& output : mpd.Outputs()) {
        bars.emplace_back(
            grid.Rows( i*2-1, i*2 ),
            window,
            Compass::EAST,
            output.Name()
          );

        auto& bar = bars.back();
        RegisterChild(&bar);

        auto& button = bar.AddButton("ENABLED");
        button.Active(output.Enabled());
        miso::connect(button.signal_press, [&output]() {
          miso::sender<Button>()->Active(
              output.EnabledToggle()
            );
        });

        i++;
      }
    }
};

class MpdFrame : public Drawable {
  protected:
    const std::string V_NOWPLAYING = "NOW PLAYING";
    const std::string V_QUEUE = "QUEUE";
    const std::string V_BROWSE = "BROWSE";
    const std::string V_ARTISTS = "ARTISTS";
    const std::string V_SEARCH = "SEARCH";
    const std::string V_OUTPUTS = "OUTPUTS";

    MPDXX::Client mpd;

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
    View viewBrowse;
    View viewArtists;
    View viewSearch;
    OutputsView viewOutputs;


  public:
    MpdFrame(Grid g, Window& w)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrFrame{layout.North(), w, Compass::EAST, " MPD CONTROL "}
        , barView{w, layout.West()}
        , playerControlBar{w, layout.South(), mpd}
        , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
        , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

        , viewNowPlaying(layout.Centre(), w, mpd)
        , viewQueue     (layout.Centre(), w, mpd)
        , viewOutputs   (layout.Centre(), w, mpd)
        , viewBrowse (V_BROWSE, layout.Centre())
        , viewArtists(V_ARTISTS, layout.Centre())
        , viewSearch (V_SEARCH, layout.Centre())
    {
      RegisterChild(&hdrFrame);
      RegisterChild(&barView);
      RegisterChild(&playerControlBar);
      RegisterChild(&sweepNorthWest);
      RegisterChild(&sweepSouthWest);

      RegisterView(&viewNowPlaying);
      RegisterView(&viewQueue);
      RegisterView(&viewBrowse);
      RegisterView(&viewArtists);
      RegisterView(&viewSearch);
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

