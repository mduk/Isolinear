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
#include "newbo.h"

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

void grid_test(Window& window) {
  window.grid.DrawCells(window.sdl_renderer);
  window.Add(new Region{window.grid.Column(2)});
  window.Add(new Region{window.grid.Row(2)});
  window.Add(new Region{window.grid.SingleCellRegion(4,4)});
  window.Add(new Region{window.grid.MultiCellRegion(4,6, 4,12)});
  window.Add(new Region{window.grid.MultiCellRegion(6,4, 12,4)});
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

  grid_test(window);

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
