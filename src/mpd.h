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

    Header  hdrSong;

  public:
    ~MpdFrame()
    {
      mpd_connection_free(conn);
      mpd_status_free(status);
      mpd_song_free(song);
    }

    MpdFrame(Grid g, Window& w)
        : Frame{ g, w, 0, 0, 2, 3 }
        , btnPlay(SouthBar().AddButton("PLAY "))
        , btnPause(SouthBar().AddButton("PAUS "))
        , btnStop(SouthBar().AddButton("STOP "))
        , btnPrevious(SouthBar().AddButton("PREV "))
        , btnNext(SouthBar().AddButton("NEXT "))
        , btnQueue(WestBar().AddButton("QUEUE "))
        , btnBrowse(WestBar().AddButton("BROWSE "))
        , btnArtists(WestBar().AddButton("ARTISTS "))
        , btnSearch(WestBar().AddButton("SEARCH "))
        , btnOutputs(WestBar().AddButton("OUTPUTS "))
        , hdrSong{g.SubGrid(1, 1, g.MaxColumns(), 2), w, " MPD CONTROL "}
    {

      conn = mpd_connection_new(NULL, 0, 30000);

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

    virtual void OnPointerEvent(PointerEvent event) override
    {
      if (NorthBar().Bounds().Encloses(event.Position())) {
        NorthBar().OnPointerEvent(event);
      }

      if (EastBar().Bounds().Encloses(event.Position())) {
        EastBar().OnPointerEvent(event);
      }

      if (SouthBar().Bounds().Encloses(event.Position())) {
        SouthBar().OnPointerEvent(event);
      }

      if (WestBar().Bounds().Encloses(event.Position())) {
        WestBar().OnPointerEvent(event);
      }
    };

    virtual ColourScheme Colours() const override {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) override {
      Frame::Colours(cs);
      hdrSong.Colours(cs);
    }
};
