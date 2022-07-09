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
#include "theme.h"
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

  geometry::vector display_size{ display.w, display.h };

  isolinear::display::window window(
      geometry::Position2D{ display },
      display_size
    );

  std::list<isolinear::window::region> squares;
  geometry::vector margin{10,10};
  geometry::vector outer_cell_size{250,250};
  geometry::vector inner_cell_size = outer_cell_size.subtract(margin).subtract(margin);
  geometry::vector grid_size{5,5};

  int ci = 1;

  for (int x=1; x<=grid_size.x; x++) {
    for (int y=1; y<=grid_size.y; y++) {
      isolinear::window::position boundsfar( window, outer_cell_size.x * x, outer_cell_size.y * y );
      isolinear::window::position boundsnear( window, boundsfar.subtract(outer_cell_size) );

      squares.emplace_back(window, boundsnear, boundsfar);
      auto& outer_cell_region = squares.back();
      outer_cell_region.fill_colour(ci % 2 == 0 ? 0xffffaa66 : 0xff66aaff);

      isolinear::window::position inner_cell_near(window, boundsnear.add(margin));
      squares.emplace_back(window, inner_cell_near, inner_cell_size);
      auto& inner_cell_region = squares.back();
      inner_cell_region.fill_colour(ci % 2 == 0 ? 0xff99cccc : 0xffccccff);

      ci++;
    }
  }


  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

  while (running) {
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

    window.Draw();

    for (auto& square : squares) {
      square.draw();
    }

    SDL_RenderPresent(window.renderer());
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

