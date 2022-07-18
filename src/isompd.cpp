#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

#include <asio.hpp>
#include <thread>

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "miso.h"

#include "init.h"
#include "theme.h"
#include "compasslayout.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "mpd.h"
#include "pointerevent.h"
#include "ui.h"
#include "display.h"


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;


  isolinear::init();
  auto work_guard = asio::make_work_guard(isolinear::io_context);
  auto display = isolinear::display::detect_displays().back();
  geometry::vector display_size{ display.w, display.h };
  geometry::vector display_position{ display.x, display.y };
  isolinear::display::window window(display_position, display_size);

  isolinear::grid grid(
      { 0, 0, display_size.x, display_size.y },
      { window.ButtonFont().Height() * 2, window.ButtonFont().Height() },
      { 10, 10 }
    );


  mpdxx::client mpdc(isolinear::io_context, "localhost", "6600");

  miso::connect(mpdc.signal_status, [&mpdc](mpdxx::status status){
    cout << "Status:\n";
    cout << status << "\n";
  });

  isompd::frame mpdframe(grid, window, mpdc);
  window.add(&mpdframe);

  miso::connect(mpdframe.signal_view_change, [&](std::string from_view, std::string to_view){
    cout << fmt::format("View changed from {} to {}\n", from_view, to_view);
    if (to_view == mpdframe.V_QUEUE     ) { mpdc.RequestQueue();       }
    if (to_view == mpdframe.V_NOWPLAYING) { mpdc.RequestCurrentSong(); }
    if (to_view == mpdframe.V_BROWSE    ) { mpdc.RequestArtistList();  }
  });

  window.colours(isolinear::theme::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  SDL_ShowCursor(true);
  mpdc.RequestStatus();

  while (running) {

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case 's': mpdc.RequestStatus(); break;

            case 'd': window.colours(isolinear::theme::debug_colours       ); break;
            case 'r': window.colours(isolinear::theme::red_alert_colours   ); break;
            case 'y': window.colours(isolinear::theme::yellow_alert_colours); break;
            case 'b': window.colours(isolinear::theme::blue_alert_colours  ); break;
            case 'n': window.colours(isolinear::theme::nightgazer_colours  ); break;

            case 'a': mpdframe.SwitchView(mpdframe.V_BROWSE    ); break;
            case 'c': mpdframe.SwitchView(mpdframe.V_NOWPLAYING); break;
            case 'q': mpdframe.SwitchView(mpdframe.V_QUEUE     ); break;
            case 'p': mpdframe.SwitchView(mpdframe.V_PLAYER    ); break;
          }
          break;

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          isolinear::geometry::position pos{x, y};
          int gx = grid.position_column_index(pos),
              gy = grid.position_row_index(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.set_title(ss.str());
          break;
        }

/*
      case SDL_FINGERDOWN:
        printf("TAP\n");
        window.on_pointer_event(pointer::event{ e.tfinger, window.size() });
        break;
*/

      case SDL_MOUSEBUTTONDOWN:
        printf("CLICK\n");
        window.on_pointer_event(pointer::event{ e.button });
        break;

      case SDL_QUIT:
        running = false;
        break;
      }
    }

    window.draw();
    SDL_RenderPresent(window.renderer());
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
