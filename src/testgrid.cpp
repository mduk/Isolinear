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
#include "elbo.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"
#include "display.h"
#include "window.h"

int gx, gy;

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

  int ncols = 15, nrows = 10;

  window.Colours(isolinear::theme::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  geometry::position pos{};

  printf("LOOP\n");
  while (running) {

    isolinear::grid grid(
        isolinear::geometry::region(0, 0, display_size.x, display_size.y),
        window.ButtonFont().Height(), // Row height
        geometry::vector(10,10),
        geometry::vector(ncols,nrows)
      );

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
            case SDLK_UP: nrows--; break;
            case SDLK_DOWN: nrows++; break;
            case SDLK_LEFT: ncols--; break;
            case SDLK_RIGHT: ncols++; break;
          }
          break;
        }

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          pos = geometry::position{x, y};
          gx = grid.position_column_index(pos),
          gy = grid.position_row_index(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());

          break;
        }

        case SDL_QUIT: {
          running = false;
          break;
        }
      }
    }

    window.Draw();

    isolinear::theme::colour cellcolour = 0xff00ffff;
    auto cell = grid.cell(gx, gy);
    if (window.region().encloses(cell)) {
      cellcolour = 0xffffff00;
    }
    cell.fill(window.renderer(), cellcolour);
    grid.Draw(window.renderer());

    SDL_RenderPresent(window.renderer()); }

  return 0; }
