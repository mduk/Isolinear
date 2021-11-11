#pragma once

#include <list>
#include <map>
#include <string>

#include <mpd/client.h>
#include <mpd/entity.h>
#include <mpd/message.h>
#include <mpd/search.h>
#include <mpd/status.h>
#include <mpd/tag.h>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"


namespace MPD {

  class Client {
    protected:
      struct mpd_connection* conn;
      struct mpd_status* status;
      struct mpd_song* song;

    public:
      Client() {
        conn = mpd_connection_new(NULL, 0, 30000);
      }

      int Status() {
        status = mpd_run_status(conn);
        return mpd_status_get_state(status);
      }

      bool IsPaused() {
        return Status() == MPD_STATE_PAUSE;
      }

      bool IsPlaying() {
        return Status() == MPD_STATE_PLAY;
      }

      bool IsStopped() {
        return Status() == MPD_STATE_STOP;
      }

      void Stop() {
        mpd_run_stop(conn);
      }

      void Play() {
        mpd_run_play(conn);
      }

      void Pause() {
        mpd_run_pause(conn, true);
      }

      void Resume() {
        mpd_run_pause(conn, false);
      }

      void Next() {
        mpd_run_next(conn);
      }

      void Previous() {
        mpd_run_previous(conn);
      }

      bool PauseToggle() {
        bool newstate = !IsPaused();
        mpd_run_pause(conn, newstate);
        return newstate;
      }

      bool ConsumeToggle() {
        status = mpd_run_status(conn);
        bool newstate = !mpd_status_get_consume(status);
        mpd_run_consume(conn, newstate);
        return newstate;
      }

      bool RandomToggle() {
        status = mpd_run_status(conn);
        bool newstate = !mpd_status_get_random(status);
        mpd_run_random(conn, newstate);
        return newstate;
      }

      ~Client() {
        mpd_connection_free(conn);
      }
  };
}

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
    Header song;
    Header album;
    Header artist;

  public:
    NowPlayingView(Window& w, Grid g)
      : View(" NOW PLAYING ", g)
      , song(g.Rows(1,2), w, " SONG ")
      , album(g.Rows(3,4), w, " ALBUM ")
      , artist(g.Rows(5,6), w, " ARTIST ")
    {
      RegisterChild(&song);
      RegisterChild(&album);
      RegisterChild(&artist);
    }
};


class MpdFrame : public Drawable {
  protected:
    const std::string V_NOWPLAYING = " NOW PLAYING ";
    const std::string V_QUEUE = " QUEUE ";
    const std::string V_BROWSE = " BROWSE ";
    const std::string V_ARTISTS = " ARTISTS ";
    const std::string V_SEARCH = " SEARCH ";
    const std::string V_OUTPUTS = " OUTPUTS ";

    MPD::Client mpd;

    CompassLayout layout;

    Header hdrSong;
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
    View viewQueue;
    View viewBrowse;
    View viewArtists;
    View viewSearch;
    View viewOutputs;


  public:
    MpdFrame(Grid g, Window& w)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrSong{layout.North(), w, " MPD CONTROL "}
        , barView{w, layout.West()}
        , barActions{w, layout.South()}
        , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
        , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

        , btnPlay(barActions.AddButton("PLAY "))
        , btnPause(barActions.AddButton("PAUS "))
        , btnStop(barActions.AddButton("STOP "))
        , btnPrevious(barActions.AddButton("PREV "))
        , btnNext(barActions.AddButton("NEXT "))
        , btnConsume(barActions.AddButton("CONSUME "))
        , btnRandom(barActions.AddButton("SHUFFLE "))

        , viewNowPlaying(w, layout.Centre())
        , viewQueue(V_QUEUE, layout.Centre())
        , viewBrowse(V_BROWSE, layout.Centre())
        , viewArtists(V_ARTISTS, layout.Centre())
        , viewSearch(V_SEARCH, layout.Centre())
        , viewOutputs(V_OUTPUTS, layout.Centre())
    {
      RegisterChild(&hdrSong);
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

      miso::connect(btnPlay.signal_press, [this]() {
        if (btnPlay.Active()) {
          if (btnPause.Active()) {
            mpd.Play();
          }
          else {
            mpd.Stop();
          }
        }
        else {
          mpd.Play();
        }

        btnPlay.Activate();
        btnPause.Deactivate();
        btnStop.Deactivate();
      });

      miso::connect(btnPause.signal_press, [this]() {
        btnPause.Active(mpd.PauseToggle());
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
      hdrSong.Label(activeView + " : MPD");

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

