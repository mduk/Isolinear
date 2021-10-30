#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "geometry.h"
#include "window.h"
#include "grid.h"
#include "shapes.h"
#include "elbo.h"
#include "header.h"
#include "sweep.h"

using namespace std;

string http_req(string url) {
  list<string> headers;
  headers.push_back("Accept: application/json");

  curlpp::Cleanup myCleanup;
  ostringstream response_stream;
  curlpp::Easy req;
  req.setOpt<curlpp::options::Url>(url);
  req.setOpt<curlpp::options::HttpHeader>(headers);
  req.setOpt<curlpp::options::WriteStream>(&response_stream);
  req.perform();

  return string(response_stream.str());
}

int main(int argc, char* argv[]) {
  printf("INIT\n");

  srand(time(NULL));

  //curlpp::initialize();
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

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

  SDL_Rect display = displays.front();

  Window window(
      Position2D{display},
      Size2D{display},
      Size2D{10,10}
    );


  int maxcols = window.grid.MaxColumns(),
      maxrows = window.grid.MaxRows();

  printf("main()::window.grid.MaxColumns() = %d\n", maxcols);
  printf("main()::window.grid.MaxRows()    = %d\n", maxrows);

  Grid hgrid = window.grid.Rows( 1,  2);
  Grid main  = window.grid.Rows( 3, -3);
  Grid fgrid = window.grid.Rows(-1,  0);

  Header header{hgrid, window, " HEADER BAR TITLE "};
         header.AddButton("04-8854");
         header.AddButton("04-5861");
         header.AddButton("04-8854");
         header.AddButton("04-5861");
         header.AddButton("04-8854");
         header.AddButton("04-8854");
         header.AddButton("04-5861");
         header.AddButton("04-5861");
  window.Add(&header);

  Header footer{fgrid, window};
  window.Add(&footer);

  Grid sweepgrid = window.grid.SubGrid(1,3, 4,5);
  Sweep sweep(window, sweepgrid);
  window.Add(&sweep);

  window.Colours(blue_alert_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  bool drawgrid = true;

  printf("LOOP\n");
  while (running) {

    int newmaxcols = window.grid.MaxColumns();
    int newmaxrows = window.grid.MaxRows();

    if (newmaxcols != maxcols || newmaxrows != maxrows) {
      maxcols = newmaxcols;
      maxrows = newmaxrows;

      printf("main()::window.grid.MaxColumns() = %d\n", maxcols);
      printf("main()::window.grid.MaxRows()    = %d\n", maxrows);
    }

    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawgrid) {
      window.grid.DrawCells(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case 'r': window.Colours(red_alert_colours); break;
            case 'y': window.Colours(yellow_alert_colours); break;
            case 'b': window.Colours(blue_alert_colours); break;

            case 'g': {
              drawgrid = !drawgrid;
              break;
            }
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

          window.SetTitle(ss.str());
          break;
        }

        case SDL_MOUSEBUTTONDOWN: {
          window.OnMouseButtonDown(e.button);
          break;
        }

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
