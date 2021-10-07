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

#include "sdlcreate.h"
#include "window.h"
#include "geometry.h"
#include "button.h"
#include "grid.h"
#include "elbo.h"

using namespace std;
using namespace curlpp::options;

bool running = false;
SDL_Window * sdl_window;
SDL_Renderer * renderer;

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

  sdl_window = createWindow(win_w, win_h);
  renderer = createRenderer(sdl_window);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  WindowRegion window_region(sdl_window);
  Window window;

  Grid grid(&window_region, 100, 10);

  list<Button> mylist;
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 1, 1));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 2, 2));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 3, 3));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 4, 4));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 5, 5));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 6, 6));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 7, 7));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 8, 8));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1, 9, 9));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1,10,10));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1,11,11));
  mylist.emplace_back(grid.CalculateCellRegion( 1, 1,12,12));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2, 1, 2));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2, 3, 4));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2, 5, 6));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2, 7, 8));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2, 9,10));
  mylist.emplace_back(grid.CalculateCellRegion( 2, 2,11,12));
  mylist.emplace_back(grid.CalculateCellRegion( 3, 3, 1, 3));
  mylist.emplace_back(grid.CalculateCellRegion( 3, 3, 4, 6));
  mylist.emplace_back(grid.CalculateCellRegion( 3, 3, 7, 9));
  mylist.emplace_back(grid.CalculateCellRegion( 3, 3,10,12));
  mylist.emplace_back(grid.CalculateCellRegion( 4, 4, 1, 4));
  mylist.emplace_back(grid.CalculateCellRegion( 4, 4, 5, 8));
  mylist.emplace_back(grid.CalculateCellRegion( 4, 4, 9,12));
  mylist.emplace_back(grid.CalculateCellRegion( 5, 5, 1, 6));
  mylist.emplace_back(grid.CalculateCellRegion( 5, 5, 7,12));
  mylist.emplace_back(grid.CalculateCellRegion( 6, 6, 1,12));


  running = true;
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

        case SDL_MOUSEBUTTONDOWN:
          window.OnMouseButtonDown(&e.button);
          break;

        case SDL_QUIT:
          running = false;
          break;

      }
    }


    for (auto i : mylist) {
      i.Draw(renderer);
    }


    SDL_RenderPresent(renderer);
  }

  return 0;
}
