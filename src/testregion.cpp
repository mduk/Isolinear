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

  isolinear::Size2D display_size{ display };

  isolinear::display::window window(
      geometry::Position2D{ display },
      display_size
    );

  std::list<isolinear::window::region> squares;
  geometry::vector margin{4,4};
  geometry::vector cell_size{40,40};
  geometry::vector grid_size{64,36};

  for (int i=1; i<=grid_size.x; i++) {
    for (int j=1; j<=grid_size.y; j++) {
      isolinear::window::position boundsfar{ window, cell_size.x*i, cell_size.y*j };
      isolinear::window::position boundsnear{ window, boundsfar.Subtract(cell_size) };

      squares.emplace_back(window, boundsnear, boundsfar);
      auto& bg = squares.back();
      bg.fill_colour(0xff990000);

      squares.emplace_back(window, boundsnear.Add(margin), boundsfar.Subtract(margin));
      auto& square = squares.back();
      square.fill_colour(0xffccaa99);
    }
  }


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
            case SDLK_ESCAPE: running = false; break;
          }
          break;

        case SDL_QUIT:
          running = false;
          break;

      }
    }

    window.Update();
    window.Draw();

    for (auto& square : squares) {
      square.draw();
    }

    SDL_RenderPresent(window.sdl_renderer);
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

