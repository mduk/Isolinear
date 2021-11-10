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


class MpdFrame : public Drawable {
  protected:
    const std::string V_NOWPLAYING = " NOW PLAYING ";
    const std::string V_QUEUE = " QUEUE ";
    const std::string V_BROWSE = " BROWSE ";
    const std::string V_ARTISTS = " ARTISTS ";
    const std::string V_SEARCH = " SEARCH ";
    const std::string V_OUTPUTS = " OUTPUTS ";

    struct mpd_connection* conn;
    struct mpd_status* status;
    struct mpd_song* song;

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

    Button& btnNowPlaying;
    Button& btnQueue;
    Button& btnBrowse;
    Button& btnArtists;
    Button& btnSearch;
    Button& btnOutputs;

    View viewNowPlaying;
    View viewQueue;
    View viewBrowse;
    View viewArtists;
    View viewSearch;
    View viewOutputs;

  public:
    ~MpdFrame()
    {
      mpd_connection_free(conn);
      mpd_status_free(status);
      mpd_song_free(song);
    }

    void RegisterView(View* view) {
      const std::string view_name = view->Name();
      views.insert(std::pair<const std::string, View*>(view_name, view));
    }

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
        , btnNowPlaying(barView.AddButton(V_NOWPLAYING))
        , btnQueue(barView.AddButton(V_QUEUE))
        , btnBrowse(barView.AddButton(V_BROWSE))
        , btnArtists(barView.AddButton(V_ARTISTS))
        , btnSearch(barView.AddButton(V_SEARCH))
        , btnOutputs(barView.AddButton(V_OUTPUTS))
        , conn(mpd_connection_new(NULL, 0, 30000))

        , viewNowPlaying(V_NOWPLAYING, layout.Centre())
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
        btnNowPlaying.Deactivate();
        btnQueue.Deactivate();
        btnBrowse.Deactivate();
        btnArtists.Deactivate();
        btnSearch.Deactivate();
        btnOutputs.Deactivate();

        auto active = miso::sender<Button>();
        active->Activate();
        activeView = active->Label();
      };
      miso::connect(btnNowPlaying.signal_press, switch_view);
      miso::connect(btnQueue.signal_press, switch_view);
      miso::connect(btnBrowse.signal_press, switch_view);
      miso::connect(btnArtists.signal_press, switch_view);
      miso::connect(btnSearch.signal_press, switch_view);
      miso::connect(btnOutputs.signal_press, switch_view);
      btnNowPlaying.Activate();

      miso::connect(btnPlay.signal_press, [this]() {
        if (btnPlay.Active()) {
          mpd_run_stop(conn);
        }
        else {
          mpd_run_play(conn);
        }

        btnPlay.Activate();
        btnPause.Deactivate();
        btnStop.Deactivate();
      });

      miso::connect(btnPause.signal_press, [this]() {
        bool isPaused = btnPause.Active();

        mpd_run_pause(conn, !isPaused);
        if (isPaused) {
          btnPause.Deactivate();
        } else {
          btnPause.Activate();
        }
      });

      miso::connect(btnStop.signal_press, [this]() {
        if (btnStop.Active()) {
          mpd_run_play(conn);
        }
        else {
          mpd_run_stop(conn);
        }
        btnPlay.Deactivate();
        btnPause.Deactivate();
        btnStop.Activate();
      });

      miso::connect(btnPrevious.signal_press, [this]() {
        mpd_run_previous(conn);
      });

      miso::connect(btnNext.signal_press, [this]() {
        mpd_run_next(conn);
      });

      Update();
    }

    void Update()
    {
      status = mpd_run_status(conn);
      song = mpd_run_current_song(conn);

      hdrSong.Label(activeView + " : MPD");

      switch (mpd_status_get_state(status)) {

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


};

