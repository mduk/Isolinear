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
  Grid grid{
    Region{Position{0,0}, window.size},
    NumCols{12},
    Margin{10,10},
    Gutter{10,10}
  };

  Elbo* mainelbo = new Elbo(
      grid.MultiCellRegion( 1, 1, 6,12),
      ColourScheme{0xff3399ff, 0xff99ffff, 0xff66ccff, 0xff3f403f},
      Size{grid.MultiCellRegion(1,1, 2,1).size.x, 20},
      InnerRadius{70}
  );
  mainelbo->AddButton();
  mainelbo->AddButton();
  mainelbo->AddButton();
  mainelbo->AddButton();
  window.Add(mainelbo);
  window.Add(new Elbo(
      grid.MultiCellRegion( 7, 1,12, 6),
      ColourScheme{0xff996600, 0xffcc99cc, 0xffffcc99, 0xffcc6633},
      Size{grid.CellSize().x, grid.CellSize().y / 3}
  ));
  window.Add(new Elbo(
      grid.MultiCellRegion( 7, 7,12,12),
      ColourScheme{0xff664466, 0xffcc9999, 0xffff9999, 0xff6666cc},
      Size{80,20},
      InnerRadius{50}
  ));

  bool running = true;
  window.Draw();
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
    }



  }

  return 0;
}
