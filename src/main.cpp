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

  int win_h = 1250,
      win_w = win_h * 1.618;

  Window window(win_w, win_h);
  Grid grid{window.size};

  window.Add(new Button(grid.SingleCellRegion(2,2)));
  window.Add(new Button(grid.SingleCellRegion(4,4)));

  bool running = true;
  while (running) {


    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;
          }
          break;

        case SDL_MOUSEBUTTONDOWN: {
          //window.OnMouseButtonDown(e.button);
          Position cursor{e.button.x, e.button.y};
          window.Add(new Button(grid.PositionRegion(cursor)));
          break;
        }

        case SDL_QUIT:
          running = false;
          break;

      }
    }


    window.Draw();

  }

  return 0;
}
