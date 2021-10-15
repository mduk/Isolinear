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

  ColourScheme colours{0xff664466, 0xffcc9999, 0xffff9999, 0xff6666cc};
  Window window(win_w, win_h);

  Elbo* main = new Elbo(
      window,
      window.grid.MultiCellRegion(1,1, 12,12),
      colours,
      Size{
          window.grid.MultiCellRegion(1,1, 2,2).size.x,
          window.grid.CellSize().y / 3
        },
      "MAIN WINDOW"
  );

  main->AddButton();
  main->AddButton();
  main->AddButton();
  main->AddButton();
  window.Add(main);

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

        case SDL_QUIT: {
          running = false;
          break;
        }
      }

      window.Draw();
      window.grid.DrawCells(window.sdl_renderer);
    }



  }

  return 0;
}
