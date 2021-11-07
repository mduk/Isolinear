#pragma once

#include <string>
#include <list>

#include <mpd/client.h>
#include <mpd/entity.h>
#include <mpd/message.h>
#include <mpd/search.h>
#include <mpd/status.h>
#include <mpd/tag.h>

#include "miso.h"

#include "geometry.h"
#include "window.h"
#include "header.h"

class MpdFrame : public Frame {
  protected:
    struct mpd_connection* conn;
    struct mpd_status* status;
    struct mpd_song* song;

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
    ~MpdFrame() {
      mpd_connection_free(conn);
      mpd_status_free(status);
      mpd_song_free(song);
    }

    MpdFrame(Grid g, Window& w)
      : Frame{ g, w, 2, 0, 0, 3 },
        btnPlay    ( NorthBar().AddButton("PLAY "   ) ),
        btnPause   ( NorthBar().AddButton("PAUS "   ) ),
        btnStop    ( NorthBar().AddButton("STOP "   ) ),
        btnPrevious( NorthBar().AddButton("PREV "   ) ),
        btnNext    ( NorthBar().AddButton("NEXT "   ) ),
        btnQueue   (  WestBar().AddButton("QUEUE "  ) ),
        btnBrowse  (  WestBar().AddButton("BROWSE " ) ),
        btnArtists (  WestBar().AddButton("ARTISTS ") ),
        btnSearch  (  WestBar().AddButton("SEARCH " ) ),
        btnOutputs (  WestBar().AddButton("OUTPUTS ") )

    {

      conn = mpd_connection_new(NULL, 0, 30000);
      status = mpd_run_status(conn);

      miso::connect(btnPlay.signal_press, [this]() {
          mpd_run_play(conn);
          btnPlay.Activate();
          btnPause.Deactivate();
          btnStop.Deactivate();
        });

      miso::connect(btnPause.signal_press, [this]() {
          bool isPaused = btnPause.Active();

          mpd_run_pause(conn, !isPaused);
          if (isPaused) {
            btnPause.Deactivate();
          }
          else {
            btnPause.Activate();
          }
        });

      miso::connect(btnStop.signal_press, [this]() {
          mpd_run_stop(conn);
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

    void Update() {
      status = mpd_run_status(conn);
      switch (mpd_status_get_state(status)) {

        case MPD_STATE_PLAY:
          btnPlay.Activate();
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
          btnPause.Deactivate();
          btnStop.Activate();
          break;

      }
    }

    virtual void OnPointerEvent(PointerEvent event) override {
      NorthBar().OnPointerEvent(event);
       EastBar().OnPointerEvent(event);
      SouthBar().OnPointerEvent(event);
       WestBar().OnPointerEvent(event);
    };

};
