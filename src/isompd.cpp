#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

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


bool drawdebug = false;

static int handle_error(struct mpd_connection* c) {
    assert(mpd_connection_get_error(c) != MPD_ERROR_SUCCESS);
    fprintf(stderr, "%s\n", mpd_connection_get_error_message(c));
    mpd_connection_free(c);
    return EXIT_FAILURE;
}

int main(int argc, char* argv[])
{
  printf("INIT\n");

  srand(time(NULL));

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
      window
    };
  window.Add(&mpdframe);

  window.Colours(nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

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

      case SDL_KEYDOWN: {
        switch (e.key.keysym.sym) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case 'c':
          SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));
          break;
        case 'd':
          window.Colours(debug_colours);
          break;
        case 'r':
          window.Colours(red_alert_colours);
          break;
        case 'y':
          window.Colours(yellow_alert_colours);
          break;
        case 'b':
          window.Colours(blue_alert_colours);
          break;
        case 'n':
          window.Colours(nightgazer_colours);
          break;
        case 'g':
          drawdebug = !drawdebug;
          break;
/*
        case 'h':
          bar1.Dec(10);
          break;

        case 'l':
          bar1.Inc(10);
          break;
*/
        }
        break;
      }

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

      case SDL_WINDOWEVENT: {
        switch (e.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
          window.OnWindowResize(e.window);
          break;
        };
        }
        break;
      }

      case SDL_QUIT: {
        running = false;
        break;
      }
      }
    }

    window.Update();
    window.Draw();

    SDL_RenderPresent(window.sdl_renderer);
  }

  return 0;
}
