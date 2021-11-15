#pragma once

#include <list>
#include <map>
#include <string>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"


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

class NowPlayingView : public View {
  protected:
    PairHeaderBar title;
    PairHeaderBar album;
    PairHeaderBar artist;
    MPDXX::Client& mpd;

  public:
    NowPlayingView(Window& w, Grid g, MPDXX::Client& _mpd)
      : View("NOW PLAYING", g)
      , mpd{_mpd}
      , title(g.Rows(3,4), w, "TITLE", "[title]")
      , album(g.Rows(5,6), w, "ALBUM", "[album]")
      , artist(g.Rows(7,8), w, "ARTIST", "[artist]")
    {
      RegisterChild(&title);
      RegisterChild(&album);
      RegisterChild(&artist);
    }

    void Update() {
      MPDXX::Song now = mpd.CurrentlyPlaying();
      title.Right(now.Title());
      album.Right(now.Album());
      artist.Right(now.Artist());
    }
};

class QueueView : public View {
  protected:
    Window& window;
    MPDXX::Client& mpd;

  public:
    QueueView(Window& w, Grid g, MPDXX::Client& _mpd)
      : View("QUEUE", g)
      , window{w}
      , mpd{_mpd}
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
    HorizontalButtonBar barActions;
    NorthWestSweep sweepNorthWest;
    SouthWestSweep sweepSouthWest;

    std::map<const std::string, View*> views;
    std::string activeView{V_NOWPLAYING};

    Button& btnPlay;
    Button& btnPause;
    Button& btnStop;
    Button& btnPrevious;
    Button& btnNext;
    Button& btnConsume;
    Button& btnRandom;

    NowPlayingView viewNowPlaying;
    QueueView viewQueue;
    View viewBrowse;
    View viewArtists;
    View viewSearch;
    View viewOutputs;


  public:
    MpdFrame(Grid g, Window& w)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrFrame{layout.North(), w, Compass::EAST, " MPD CONTROL "}
        , barView{w, layout.West()}
        , barActions{w, layout.South()}
        , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
        , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

        , btnPlay(barActions.AddButton("PLAY"))
        , btnPause(barActions.AddButton("PAUSE"))
        , btnStop(barActions.AddButton("STOP"))
        , btnPrevious(barActions.AddButton("PREVIOUS"))
        , btnNext(barActions.AddButton("NEXT"))
        , btnConsume(barActions.AddButton("CONSUME"))
        , btnRandom(barActions.AddButton("SHUFFLE"))

        , viewNowPlaying(w, layout.Centre(), mpd)
        , viewQueue     (w, layout.Centre(), mpd)
        , viewBrowse (V_BROWSE, layout.Centre())
        , viewArtists(V_ARTISTS, layout.Centre())
        , viewSearch (V_SEARCH, layout.Centre())
        , viewOutputs(V_OUTPUTS, layout.Centre())
    {
      RegisterChild(&hdrFrame);
      RegisterChild(&barView);
      RegisterChild(&barActions);
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

      miso::connect(btnPlay.signal_press, [this]() {
        if (mpd.IsStopped()) {
          mpd.Play();

          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Active();

          btnStop.Enable();
          btnStop.Deactivate();
          return;
        }

        if (mpd.IsPaused()) {
          mpd.Resume();

          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Activate();

          btnStop.Enable();
          btnStop.Deactivate();
          return;
        }

        if (mpd.IsPlaying()) {
          mpd.Stop();

          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Activate();

          btnStop.Enable();
          btnStop.Deactivate();
          return;
        }
      });

      miso::connect(btnStop.signal_press, [this]() {
        if (btnStop.Active()) {
          mpd.Play();
        }
        else {
          mpd.Stop();
        }
        btnPlay.Deactivate();
        btnPause.Deactivate();
        btnStop.Activate();
      });

      miso::connect(btnPrevious.signal_press, [this]() {
        mpd.Previous();
      });

      miso::connect(btnNext.signal_press, [this]() {
        mpd.Next();
      });

      miso::connect(btnPause.signal_press, [this]() {
        miso::sender<Button>()->Active(mpd.PauseToggle());
      });

      miso::connect(btnConsume.signal_press, [this]() {
        miso::sender<Button>()->Active(mpd.ConsumeToggle());
      });

      miso::connect(btnRandom.signal_press, [this]() {
        miso::sender<Button>()->Active(mpd.RandomToggle());
      });

      Update();
    }

    void Update()
    {
      hdrFrame.Label(activeView + " : MPD " + mpd.StatusString());

      switch (mpd.Status()) {

      case MPD_STATE_PLAY:
        btnPlay.Activate();
        btnPause.Enable();
        btnPause.Deactivate();
        btnStop.Deactivate();
        break;

      case MPD_STATE_PAUSE:
        btnPlay.Activate();
        btnPause.Activate();
        btnStop.Deactivate();
        break;

      case MPD_STATE_STOP:
        btnPlay.Deactivate();
        btnPause.Disable();
        btnStop.Activate();
        break;
      }

      btnConsume.Active(mpd.Consume());
      btnRandom.Active(mpd.Random());

      views.at(activeView)->Update();
    }

    virtual Region2D Bounds() const override {
      return layout.Bounds();
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

