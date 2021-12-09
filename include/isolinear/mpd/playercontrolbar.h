#pragma once

#include <list>
#include <map>
#include <string>

#include "miso.h"

#include <iostream>
#include <fmt/core.h>

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"
#include "buttonbar.h"


using std::cout;


class PlayerControlBar : public HorizontalButtonBar {
  protected:
    mpdxx::client& mpd;

    Button& btnPlay;
    Button& btnPause;
    Button& btnStop;
    Button& btnPrevious;
    Button& btnNext;
    Button& btnConsume;
    Button& btnRandom;
    Button& btnSingle;
    Button& btnRepeat;

    int queue_length = 0;

  public:
    PlayerControlBar(Window& w, Grid g, mpdxx::client& _mpd)
      : HorizontalButtonBar(w, g)
      , mpd{_mpd}
      , btnPlay(AddButton("PLAY"))
      , btnPause(AddButton("PAUSE"))
      , btnStop(AddButton("STOP"))
      , btnPrevious(AddButton("PREVIOUS"))
      , btnNext(AddButton("NEXT"))
      , btnConsume(AddButton("CONSUME"))
      , btnRandom(AddButton("RANDOM"))
      , btnSingle(AddButton("SINGLE"))
      , btnRepeat(AddButton("REPEAT"))
    {
      miso::connect(mpd.signal_queue, [this](std::list<mpdxx::stringmap> queue){
        queue_length = queue.size();
        cout << fmt::format("received new queue {}\n", queue_length);
        for (auto& song : queue) {
          cout << song.at("Title") << "\n";
        }
      });

      miso::connect(mpd.signal_status, [this](mpdxx::status status){
        cout << fmt::format("PlayerControlBar signal_status begin\n");
        btnConsume.Active(status.Consume());
        btnRandom.Active(status.Random());
        btnSingle.Active(status.Single());
        btnRepeat.Active(status.Repeat());

        if (status.IsPlaying()) {
          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Deactivate();

          btnStop.Enable();
          btnStop.Deactivate();
        }

        if (status.IsPaused()) {
          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Activate();

          btnStop.Enable();
          btnStop.Deactivate();
        }

        if (status.IsStopped()) {
          btnStop.Enable();
          btnStop.Activate();

          btnPause.Disable();

          btnPlay.Deactivate();
          if (queue_length > 0) {
            btnPlay.Disable();
          } else {
            btnPlay.Enable();
          }
        }

        cout << fmt::format("PlayerControlBar signal_status end\n");
      });

      miso::connect(    btnPlay.signal_press, [this]() { mpd.Play();          });
      miso::connect(    btnStop.signal_press, [this]() { mpd.Stop();          });
      miso::connect(btnPrevious.signal_press, [this]() { mpd.Previous();      });
      miso::connect(    btnNext.signal_press, [this]() { mpd.Next();          });
      miso::connect(   btnPause.signal_press, [this]() { mpd.TogglePause();   });
      miso::connect( btnConsume.signal_press, [this]() { mpd.ToggleConsume(); });
      miso::connect(  btnRandom.signal_press, [this]() { mpd.ToggleRandom();  });
      miso::connect(  btnRandom.signal_press, [this]() { mpd.ToggleSingle();  });
      miso::connect(  btnRandom.signal_press, [this]() { mpd.ToggleRepeat();  });
    }

    void Update() { }
};
