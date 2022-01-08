#include <sstream>
#include <vector>

#include <asio.hpp>
#include <thread>

#include <iostream>
#include <fmt/core.h>
#include <fmt/chrono.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "miso.h"

#include "init.h"
#include "colours.h"
#include "drawable.h"
#include "geometry.h"
#include "pointerevent.h"
#include "ui.h"
#include "display.h"
#include "window.h"


using std::cout;


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;


  isolinear::init();

  auto work_guard = asio::make_work_guard(isolinear::io_context);
  auto display = isolinear::display::detect_displays().back();

  Size2D display_size{ display };

  isolinear::display::window window(
      geometry::Position2D{ display },
      display_size
    );

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));

  while (running) {
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
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

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

