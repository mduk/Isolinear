#pragma once

#include <list>
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

void generichandler() {
  Button* sender = miso::sender<Button>();
  if (sender->Active()) {
    sender->Deactivate();
  }
  else {
    sender->Activate();
  }
}

class MpdFrame : public Drawable {
  protected:
    struct mpd_connection* conn;
    struct mpd_status* status;
    struct mpd_song* song;

    CompassLayout layout;

    Header hdrSong;
    VerticalButtonBar barView;
    HorizontalButtonBar barActions;
    NorthWestSweep sweepNorthWest;
    SouthWestSweep sweepSouthWest;

    Button& btnPlay;
    Button& btnPause;
    Button& btnStop;
    Button& btnPrevious;
    Button& btnNext;

    Button& btnQueue;
    Button& btnBrowse;
    Button& btnArtists;
    Button& btnSearch;
    Button& btnOutputs;


  public:
    ~MpdFrame()
    {
      mpd_connection_free(conn);
      mpd_status_free(status);
      mpd_song_free(song);
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
        , btnQueue(barView.AddButton("QUEUE "))
        , btnBrowse(barView.AddButton("BROWSE "))
        , btnArtists(barView.AddButton("ARTISTS "))
        , btnSearch(barView.AddButton("SEARCH "))
        , btnOutputs(barView.AddButton("OUTPUTS "))
    {
      RegisterChild(&hdrSong);
      RegisterChild(&barView);
      RegisterChild(&barActions);
      RegisterChild(&sweepNorthWest);
      RegisterChild(&sweepSouthWest);

      conn = mpd_connection_new(NULL, 0, 30000);

      btnQueue.Activate();

      miso::connect(btnQueue.signal_press, generichandler);
      miso::connect(btnBrowse.signal_press, generichandler);
      miso::connect(btnArtists.signal_press, generichandler);
      miso::connect(btnSearch.signal_press, generichandler);
      miso::connect(btnOutputs.signal_press, generichandler);

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
    }


    virtual Region2D Bounds() const override {
      return layout.Bounds();
    }

};
