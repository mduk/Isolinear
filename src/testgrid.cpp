#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "miso.h"

#include "init.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"
#include "display.h"
#include "window.h"


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;


  isolinear::init();
  auto work_guard = asio::make_work_guard(isolinear::io_context);
  auto display = isolinear::display::detect_displays().back();
  geometry::vector display_position{ display.x, display.y };
  geometry::vector display_size{ display.w, display.h };
  isolinear::display::window window(display_position, display_size);

  int cell_scale  = 30;

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  geometry::position pointer_position{};
  int font_height = window.ButtonFont().Height();

    isolinear::grid grid(
        { 0, 0, display_size.x, display_size.y }, // Display Region
        { cell_scale*2, cell_scale }, // Cell Size
        { 6, 6 } // Cell Gutter
      );


  printf("LOOP\n");
  while (running) {

    window.colours(isolinear::theme::nightgazer_colours);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case SDLK_UP:
              cell_scale--;
              grid.set_cell_size({ cell_scale*2, cell_scale });
             break;

            case SDLK_DOWN:
              cell_scale++;
              grid.set_cell_size({cell_scale*2, cell_scale});
              break;
          }
          break;
        }

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          pointer_position = geometry::position{x, y};

          std::stringstream ss;
          ss << "Mouse X=" << x
             << " Y=" << y
             << " Grid Col=" << grid.position_column_index(pointer_position)
             << " Row=" << grid.position_row_index(pointer_position);

          window.Title(ss.str());

          break;
        }

        case SDL_QUIT: {
          running = false;
          break;
        }
      }
    }


    grid.draw(window.renderer());
    grid.left_columns(2).draw(window.renderer());
    grid.right_columns(2).draw(window.renderer());
    grid.centre_columns(8,8).draw(window.renderer());
    grid.top_rows(1).draw(window.renderer());
    grid.bottom_rows(1).draw(window.renderer());
    grid.centre_rows(8,8).draw(window.renderer());

    window.draw();

    isolinear::theme::colour cellcolour = 0xff00ffff;
    try {
      auto cell = grid.cell(
        grid.position_column_index(pointer_position),
        grid.position_row_index(pointer_position)
      );

      if (window.region().encloses(cell)) {
        cellcolour = 0xffffff00;
      }

      cell.fill(window.renderer(), cellcolour);
    }
    catch (const std::out_of_range& e) {
      // Do nothing, it's fine.
    }

    SDL_RenderPresent(window.renderer());
  } // while (running)

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
