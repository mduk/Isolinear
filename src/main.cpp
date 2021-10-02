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

  Region* window_region = new WindowRegion(window);
  int row_height = 100;

  int elbo_outer_radius = 90;
  int elbo_inner_radius = 60;
  int elbo_hline_thickness = 15;
  Position* elbo_corner = new Position(elbo_outer_radius*2, elbo_hline_thickness);

  Grid* grid = new Grid(window_region, row_height, 10);
  Elbo* elbo1 = new Elbo(grid->CalculateCellRegion(1,14,1, 6), elbo_corner, 0xff3399ff);
  Elbo* elbo2 = new Elbo(grid->CalculateCellRegion(7,14,7,12), elbo_corner, 0xff3399ff);
  Elbo* elbo3 = new Elbo(grid->CalculateCellRegion(1,6, 7,12), elbo_corner, 0xff3399ff);

  int row, col;
  for (row=1; row<=36; row++) {
    for (col=1; col<=12; col++) {
      Button* button = new Button();
      button->c = RandomColour();
      grid->AssignRegion(row, row, col, col, button);
    }
  }

  running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
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

    elbo1->Draw(renderer); elbo1->ContainerRegion()->Draw(renderer);
    elbo2->Draw(renderer); elbo2->ContainerRegion()->Draw(renderer);
    elbo3->Draw(renderer); elbo3->ContainerRegion()->Draw(renderer);

    SDL_RenderPresent(renderer);
  }

  delete grid;
  delete elbo1, elbo2, elbo3;
  delete elbo_corner;

  return 0;
}
