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
      , btnPlay(add_button("PLAY"))
      , btnPause(add_button("PAUSE"))
      , btnStop(add_button("STOP"))
      , btnPrevious(add_button("PREVIOUS"))
      , btnNext(add_button("NEXT"))
    {
      miso::connect(mpd.signal_status, [this](mpdxx::status status){
        cout << fmt::format("PlayerControlBar signal_status begin\n");

        if (status.IsPlaying()) {
          cout << " => Playing\n";

          btnPlay.enable();
          btnPlay.activate();

          btnPause.enable();
          btnPause.deactivate();

          btnStop.enable();
          btnStop.deactivate();
        }

        if (status.IsPaused()) {
          cout << " => Paused\n";

          btnPlay.enable();
          btnPlay.enable();
          btnPlay.activate();

          btnPause.enable();
          btnPause.activate();

          btnStop.enable();
          btnStop.deactivate();
        }

        if (status.IsStopped()) {
          cout << " => Stopped\n";

          btnPlay.enable();
          btnStop.enable();
          btnStop.activate();

          btnPause.disable();

          btnPlay.deactivate();
          if (queue_length > 0) {
            btnPlay.disable();
          } else {
            btnPlay.enable();
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
