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
#include "geometry.h"
#include "button.h"
#include "grid.h"
#include "elbo.h"

using namespace std;
using namespace curlpp::options;

bool running = false;
SDL_Window *window;
SDL_Renderer *renderer;

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

  window = createWindow(win_w, win_h);
  renderer = createRenderer(window);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  SDL_GetWindowSize(window, &win_w, &win_h);
  Region* window_region = new Region(0, 0, win_w, win_h);
  int row_height = 100;

  Position* elbo_inner_corner = new Position(
      row_height * 1.618 * 2,
      row_height / 2
  );

  Elbo* elbo = new Elbo(window_region, elbo_inner_corner, 0xff3399ff);
  Grid* grid = new Grid(elbo->ContainerRegion(), 100, 5);

  int row, col;
  for (row=1; row<=12; row++) {
    Region* lcap_region = grid->CalculateLeftCapRegion(row);
    Button* lcap_button = new Button();
    lcap_button->lcap = true;
    grid->AssignRegion(lcap_region, lcap_button);

    for (col=1; col<=12; col++) {
      Button* button = new Button();
      button->c = RandomColour();
      grid->AssignRegion(row, row, col, col, button);
    }

    Region* rcap_region = grid->CalculateRightCapRegion(row);
    Button* rcap_button = new Button();
    rcap_button->rcap = true;
    grid->AssignRegion(rcap_region, rcap_button);
  }

  running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case 'q':
              running = false;
              break;
          }
          break;

        case SDL_MOUSEBUTTONDOWN:
          grid->OnMouseButtonDown(&e.button);
          break;

        case SDL_QUIT:
          running = false;
          break;

      }
    }

    elbo->Draw(renderer);
    grid->Draw(renderer);
    SDL_RenderPresent(renderer);
  }

  delete grid;
  delete elbo_inner_corner;
  delete elbo;

  return 0;
}
