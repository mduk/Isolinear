#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "miso.h"

#include "frame.h"
#include "geometry.h"
#include "window.h"
#include "grid.h"
#include "shapes.h"
#include "elbo.h"
#include "header.h"
#include "buttonbar.h"
#include "sweep.h"
#include "pointerevent.h"

using namespace std;

bool drawdebug = false;

int main(int argc, char* argv[]) {
  printf("INIT\n");

  srand(time(NULL));

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_ShowCursor(SDL_DISABLE);

  int number_of_displays = SDL_GetNumVideoDisplays();
  std::vector<SDL_Rect> displays;

  for (int i=0; i<number_of_displays; i++) {
    SDL_Rect bounds{};
    SDL_GetDisplayBounds(i, &bounds);
    displays.push_back(bounds);

    printf("%d: %d,%d +(%d,%d) [%d]\n",
        i,
        bounds.w,
        bounds.h,
        bounds.x,
        bounds.y,
        bounds.w * bounds.h
      );
  }

  SDL_Rect display = displays.back();

  Window window(
      Position2D{display},
      Size2D{display}
    );

  Header header{window.grid.Rows(1,2), window, " MPD CONTROL "};
  miso::connect(header.AddButton("PREV ").signal_press, []() {
      printf("MPD Previous\n");
    });
  window.Add(&header);

  /*
  Frame frame1{window.grid.Rows( 4, 12), window, 0, 2, 2, 2};
  window.Add(&frame1);

  Frame frame2{window.grid.Rows(13, 21), window, 2, 2, 0, 2};
  window.Add(&frame2);

  Button btn{window, window.grid.Rows(1,2).bounds, blue_alert_colours, "ONE"};
  window.Add(&btn);

  miso::connect(btn.signal_activate, [](std::string label) {
      printf("activate %s\n", label.c_str());
    });
  miso::connect(btn.signal_deactivate, [](std::string label) {
      printf("deactivate %s\n", label.c_str());
    });
  */

  window.Colours(blue_alert_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

  printf("LOOP\n");
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
            case SDLK_ESCAPE: running = false; break;
            case         'c': SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY)); break;
            case         'd': window.Colours(debug_colours); break;
            case         'r': window.Colours(red_alert_colours); break;
            case         'y': window.Colours(yellow_alert_colours); break;
            case         'b': window.Colours(blue_alert_colours); break;
            case         'g': drawdebug = !drawdebug; break;
          }
          break;
        }
/*
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
*/
        case SDL_FINGERDOWN:      window.OnPointerEvent(PointerEvent{e.tfinger, window.size}); break;
        case SDL_MOUSEBUTTONDOWN: window.OnPointerEvent(PointerEvent{e.button }); break;

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

    window.Draw();

    SDL_RenderPresent(window.sdl_renderer);
  }

  return 0;
}
