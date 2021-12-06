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

#include "buttonbar.h"
#include "elbo.h"
#include "compasslayout.h"
#include "geometry.h"
#include "grid.h"
#include "header.h"
#include "pointerevent.h"
#include "shapes.h"
#include "sweep.h"
#include "window.h"
#include "mpd.h"
#include "progressbar.h"


using asio::ip::tcp;


bool drawdebug = false;


int main(int argc, char* argv[])
{
  printf("INIT\n");

  srand(time(NULL));

  asio::io_context io_context;
  auto work_guard = asio::make_work_guard(io_context);

  std::vector<std::thread> thread_pool;
  for (size_t i = 0; i < std::thread::hardware_concurrency(); i++){
    thread_pool.emplace_back([&io_context](){ io_context.run(); });
  }

  mpdxx::Client mpdc(io_context);

  miso::connect(mpdc.signal_status, [&mpdc](mpdxx::StringMap status){
    cout << "Status:\n";
    for (auto const& [ key, val ] : status) {
      cout << fmt::format("  - {} = {}\n", key, val);
    }
  });

  mpdc.Connect("localhost", "6600");

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_ShowCursor(SDL_DISABLE);

  int number_of_displays = SDL_GetNumVideoDisplays();
  std::vector<SDL_Rect> displays;

  for (int i = 0; i < number_of_displays; i++) {
    SDL_Rect bounds{};
    SDL_GetDisplayBounds(i, &bounds);
    displays.push_back(bounds);

    printf("%d: %d,%d +(%d,%d) [%d]\n",
        i,
        bounds.w,
        bounds.h,
        bounds.x,
        bounds.y,
        bounds.w * bounds.h);
  }

  SDL_Rect display = displays.back();

  Window window(
      Position2D{ display },
      Size2D{ display }
    );

  MpdFrame mpdframe{
      window.grid,
      window,
      mpdc
    };
  window.Add(&mpdframe);

  window.Colours(nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));

  while (running) {
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawdebug) {
      window.grid.Draw(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
            case 'd': window.Colours(debug_colours); break;
            case 'r': window.Colours(red_alert_colours); break;
            case 'y': window.Colours(yellow_alert_colours); break;
            case 'b': window.Colours(blue_alert_colours); break;
            case 'n': window.Colours(nightgazer_colours); break;
            case 'g': drawdebug = !drawdebug; break;
            case 's': mpdc.RequestStatus(); break;
            case 'c': mpdc.RequestCurrentSong(); break;
            case 'q': mpdc.RequestQueue(); break;
          }
          break;

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          Position2D pos{x, y};
          int gx = window.grid.PositionColumnIndex(pos),
              gy = window.grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());
          break;
        }

/*
      case SDL_FINGERDOWN:
        printf("TAP\n");
        window.OnPointerEvent(PointerEvent{ e.tfinger, window.size });
        break;
*/

      case SDL_MOUSEBUTTONDOWN:
        printf("CLICK\n");
        window.OnPointerEvent(PointerEvent{ e.button });
        break;

      case SDL_WINDOWEVENT:
        switch (e.window.event) {
          case SDL_WINDOWEVENT_RESIZED: window.OnWindowResize(e.window); break;
        }
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

  io_context.stop();
  work_guard.reset();
  for(auto& thread : thread_pool) {
    thread.join();
  }

  return 0;
}
