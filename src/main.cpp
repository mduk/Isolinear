#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <time.h>

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

using namespace std;
using namespace curlpp::options;

string http_req(string url) {
  list<string> headers;
  headers.push_back("Accept: application/json");

  curlpp::Cleanup myCleanup;
  ostringstream response_stream;
  curlpp::Easy req;
  req.setOpt<Url>(url);
  req.setOpt<HttpHeader>(headers);
  req.setOpt<WriteStream>(&response_stream);
  req.perform();

  return string(response_stream.str());
}

int main(int argc, char* argv[]) {
  printf("INIT\n");

  srand(time(NULL));

  //curlpp::initialize();
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  Window window(
      200, 200, // Just seed values. Window immediately resizes
      Size2D{10,10},
      Size2D{10,10}
    );


  int maxcols = window.grid.MaxColumns(),
      maxrows = window.grid.MaxRows();

  printf("main()::window.grid.MaxColumns() = %d\n", maxcols);
  printf("main()::window.grid.MaxRows()    = %d\n", maxrows);

  Grid swelbogrid = window.grid.SubGrid(1,1, window.grid.MaxColumns(),4);
  Grid nwelbogrid = window.grid.SubGrid(1,5, window.grid.MaxColumns(),24);

  SouthWestElbo swelbo{ window, swelbogrid, "D-BUS: SYSTEM BUS" };
  NorthWestElbo nwelbo{ window, nwelbogrid, "D-BUS: USER BUS" };

  swelbo.AddButton("ONE");
  nwelbo.AddButton("ONE");
  nwelbo.AddButton("TWO");
  nwelbo.AddButton("THREE");

  window.Add(&swelbo);
  window.Add(&nwelbo);

  ColourScheme colours{
      0xff664466, 0xffcc9999,
      0xffff9999, 0xff6666cc
    };

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  bool drawgrid = false;

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
