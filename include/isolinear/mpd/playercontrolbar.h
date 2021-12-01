#pragma once

#include <list>
#include <map>
#include <string>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"
#include "buttonbar.h"


class PlayerControlBar : public HorizontalButtonBar {
  protected:
    mpdxx::Client& mpd;

    Button& btnPlay;
    Button& btnPause;
    Button& btnStop;
    Button& btnPrevious;
    Button& btnNext;
    Button& btnConsume;
    Button& btnRandom;

  public:
    PlayerControlBar(Window& w, Grid g, mpdxx::Client& _mpd)
      : HorizontalButtonBar(w, g)
      , mpd{_mpd}
      , btnPlay(AddButton("PLAY"))
      , btnPause(AddButton("PAUSE"))
      , btnStop(AddButton("STOP"))
      , btnPrevious(AddButton("PREVIOUS"))
      , btnNext(AddButton("NEXT"))
      , btnConsume(AddButton("CONSUME"))
      , btnRandom(AddButton("RANDOM"))
    {
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
//        miso::sender<Button>()->Active(mpd.PauseToggle());
      });

      miso::connect(btnConsume.signal_press, [this]() {
        miso::sender<Button>()->Active(mpd.ConsumeToggle());
      });

      miso::connect(btnRandom.signal_press, [this]() {
        miso::sender<Button>()->Active(mpd.RandomToggle());
      });
    }

    void Update() {
      if (mpd.IsPlaying()) {
        btnPlay.Activate();
        btnPause.Enable();
        btnPause.Deactivate();
        btnStop.Deactivate();
      }

      if (mpd.IsPaused()) {
        btnPlay.Activate();
        btnPause.Activate();
        btnStop.Deactivate();
      }

      if (mpd.IsStopped()) {
        btnPlay.Deactivate();
        btnPause.Disable();
        btnStop.Activate();
      }

      btnConsume.Active(mpd.Consume());
      btnRandom.Active(mpd.Random());
    }
};
