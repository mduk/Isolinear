#pragma once

#include <list>
#include <map>
#include <string>

#include "miso.h"

#include <iostream>
#include <fmt/core.h>

#include "geometry.h"
#include "display.h"
#include "mpdxx.h"


using std::cout;

using isolinear::ui::horizontal_button_bar;
namespace display = isolinear::display;


class PlayerControlBar : public horizontal_button_bar {
  protected:
    mpdxx::client& mpd;

    isolinear::ui::button& btnPlay;
    isolinear::ui::button& btnPause;
    isolinear::ui::button& btnStop;
    isolinear::ui::button& btnPrevious;
    isolinear::ui::button& btnNext;

    int queue_length = 0;

  public:
    PlayerControlBar(display::window& w, isolinear::grid g, mpdxx::client& _mpd)
      : horizontal_button_bar(w, g)
      , mpd{_mpd}
      , btnPlay(AddButton("PLAY"))
      , btnPause(AddButton("PAUSE"))
      , btnStop(AddButton("STOP"))
      , btnPrevious(AddButton("PREVIOUS"))
      , btnNext(AddButton("NEXT"))
    {
      miso::connect(mpd.signal_status, [this](mpdxx::status status){
        cout << fmt::format("PlayerControlBar signal_status begin\n");

        if (status.IsPlaying()) {
          cout << " => Playing\n";

          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Deactivate();

          btnStop.Enable();
          btnStop.Deactivate();
        }

        if (status.IsPaused()) {
          cout << " => Paused\n";

          btnPlay.Enable();
          btnPlay.Enable();
          btnPlay.Activate();

          btnPause.Enable();
          btnPause.Activate();

          btnStop.Enable();
          btnStop.Deactivate();
        }

        if (status.IsStopped()) {
          cout << " => Stopped\n";

          btnPlay.Enable();
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
    }

};
