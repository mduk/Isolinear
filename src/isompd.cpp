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
#include "colours.h"
#include "compasslayout.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "mpd.h"
#include "pointerevent.h"
#include "ui.h"
#include "display.h"


bool drawdebug = false;


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
      isolinear::geometry::Region2D(0, 0, display_size.x, display_size.y),
      window.ButtonFont().Height(), // Row height
      geometry::vector(10,10),
      geometry::vector(25,28)
    );


  mpdxx::client mpdc(isolinear::io_context, "localhost", "6600");

  miso::connect(mpdc.signal_status, [&mpdc](mpdxx::status status){
    cout << "Status:\n";
    cout << status << "\n";
  });

  isompd::frame mpdframe(grid, window, mpdc);
  window.Add(&mpdframe);

  miso::connect(mpdframe.signal_view_change, [&](std::string from_view, std::string to_view){
    cout << fmt::format("View changed from {} to {}\n", from_view, to_view);
    if (to_view == mpdframe.V_QUEUE     ) { mpdc.RequestQueue();       }
    if (to_view == mpdframe.V_NOWPLAYING) { mpdc.RequestCurrentSong(); }
    if (to_view == mpdframe.V_BROWSE    ) { mpdc.RequestArtistList();  }
  });

  window.Colours(isolinear::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));
  mpdc.RequestStatus();

  while (running) {
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawdebug) {
      grid.Draw(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case 'g': drawdebug = !drawdebug; break;
            case 's': mpdc.RequestStatus(); break;

            case 'd': window.Colours(isolinear::debug_colours       ); break;
            case 'r': window.Colours(isolinear::red_alert_colours   ); break;
            case 'y': window.Colours(isolinear::yellow_alert_colours); break;
            case 'b': window.Colours(isolinear::blue_alert_colours  ); break;
            case 'n': window.Colours(isolinear::nightgazer_colours  ); break;

            case 'a': mpdframe.SwitchView(mpdframe.V_BROWSE    ); break;
            case 'c': mpdframe.SwitchView(mpdframe.V_NOWPLAYING); break;
            case 'q': mpdframe.SwitchView(mpdframe.V_QUEUE     ); break;
            case 'p': mpdframe.SwitchView(mpdframe.V_PLAYER    ); break;
          }
          break;

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          isolinear::geometry::Position2D pos{x, y};
          int gx = grid.PositionColumnIndex(pos),
              gy = grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());
          break;
        }

/*
      case SDL_FINGERDOWN:
        printf("TAP\n");
        window.OnPointerEvent(pointer::event{ e.tfinger, window.size });
        break;
*/

      case SDL_MOUSEBUTTONDOWN:
        printf("CLICK\n");
        window.OnPointerEvent(pointer::event{ e.button });
        break;

      case SDL_QUIT:
        running = false;
        break;
      }
    }

    window.Update();
    window.Draw();

    SDL_RenderPresent(window.sdl_renderer);
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
