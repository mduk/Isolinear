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
  srand(time(NULL));

  curlpp::initialize();

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();

  int win_h = 1250,
      win_w = win_h * 1.618;

  Window window(
      win_w, win_h,
      12,
      Size{10,10},
      Size{10,10}
    );

  if (false) {
    GridRegion column = window.grid.Column(2);
    window.Add(&column);

    GridRegion row = window.grid.Row(2);
    window.Add(&row);

    GridRegion singlecell = window.grid.SingleCellRegion(4,4);
    window.Add(&singlecell);

    GridRegion multicell1 = window.grid.MultiCellRegion(4,6, 4,12);
    window.Add(&multicell1);

    GridRegion multicell2 = window.grid.MultiCellRegion(6,4, 12,4);
    window.Add(&multicell2);
  }

  if (true) {
    GridRegion sweep = window.grid.MultiCellRegion(1,1, 3,2);
    window.Add(&sweep);

    GridRegion horizontal = window.grid.MultiCellRegion(4,1, 12,2);
    window.Add(&horizontal);

    GridRegion vertical = window.grid.MultiCellRegion(1,3, 2,12);
    window.Add(&vertical);

    GridRegion container = window.grid.MultiCellRegion(3,3, 12,12);
    window.Add(&container);
  }

  SDL_RenderPresent(window.sdl_renderer);

  bool running = true;
  while (running) {

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
          }
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

    window.grid.DrawCells(window.sdl_renderer);
    window.Draw();
  }

  return 0;
}
