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


bool drawdebug = true;

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

  isolinear::grid grid(
      isolinear::geometry::Region2D(0, 0, display_size.x, display_size.y),
      window.ButtonFont().Height(), // Row height
      geometry::vector(10,10),
      geometry::vector(25,28)
    );


  window.Colours(isolinear::theme::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  geometry::Position2D pos{};

  printf("LOOP\n");
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

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
          case SDLK_ESCAPE:
            running = false;
            break;
          case 'c':
            SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));
            break;
          case 'd':
            window.Colours(isolinear::theme::debug_colours);
            break;
          case 'r':
            window.Colours(isolinear::theme::red_alert_colours);
            break;
          case 'y':
            window.Colours(isolinear::theme::yellow_alert_colours);
            break;
          case 'b':
            window.Colours(isolinear::theme::blue_alert_colours);
            break;
          case 'n':
            window.Colours(isolinear::theme::nightgazer_colours);
            break;
          case 'g':
            drawdebug = !drawdebug;
            break;
          }
          break;
        }

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          pos = geometry::Position2D{x, y};
          int gx = grid.PositionColumnIndex(pos),
              gy = grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());

          break;
        }

        case SDL_FINGERDOWN:
          window.OnPointerEvent(pointer::event{ e.tfinger, window.size });
          break;
        case SDL_MOUSEBUTTONDOWN:
          window.OnPointerEvent(pointer::event{ e.button });
          break;

        case SDL_QUIT: {
          running = false;
          break;
        }
      }
    }

    window.Update();
    window.Draw();
    grid.Cell(grid.PositionColumnIndex(pos), grid.PositionRowIndex(pos))
      .Fill(window.sdl_renderer, 0xff00ffff);

    SDL_RenderPresent(window.sdl_renderer);
  }

  return 0;
}
