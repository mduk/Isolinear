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
    Button& btnSingle;
    Button& btnRepeat;

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
      , btnSingle(AddButton("SINGLE"))
      , btnRepeat(AddButton("REPEAT"))
    {
      miso::connect(mpd.signal_status, [this](mpdxx::StringMap status){
        btnConsume.Active(status.at("consume") == "1");
        btnRandom.Active(status.at("random") == "1");
        btnSingle.Active(status.at("single") == "1");
        btnRepeat.Active(status.at("repeat") == "1");

        if (status.at("state") == "play") {
          btnPlay.Activate();

          btnPause.Enable();
          btnStop.Enable();

          btnPause.Deactivate();
          btnStop.Deactivate();
        }

        if (status.at("state") == "pause") {
          btnPlay.Activate();
          btnPause.Activate();

          btnStop.Deactivate();
        }

        if (status.at("state") == "stop") {
          btnStop.Activate();

          btnPause.Disable();

          btnPlay.Deactivate();
        }

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
