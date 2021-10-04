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

  Window* window = new Window();

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
          window->OnMouseButtonDown(&e.button);
          break;

        case SDL_QUIT:
          running = false;
          break;

      }
    }

    int rowh = 100;
    int gutter = 10;

    Grid* grid = new Grid(window_region, row_height, 10);

    Elbo* elbo1 = new Elbo(grid->CalculateCellRegion( 1,14, 1, 6), elbo_corner, 0xff3399ff);
    Elbo* elbo2 = new Elbo(grid->CalculateCellRegion( 7,14, 7,12), elbo_corner, 0xff3399ff);
    Elbo* elbo3 = new Elbo(grid->CalculateCellRegion( 1, 6, 7,12), elbo_corner, 0xff3399ff);
    window->Add(elbo1);
    window->Add(elbo2);
    window->Add(elbo3);

    Grid*   grid1    = new Grid(elbo1->ContainerRegion(), rowh, gutter);
    window->Add(new Button(grid1->CalculateCellRegion(1,1,1,2)));
    window->Add(new Button(grid1->CalculateCellRegion(2,2,2,2)));
    window->Add(new Button(grid1->CalculateCellRegion(3,3,3,3)));
    window->Add(new Button(grid1->CalculateCellRegion(4,4,4,4)));
    window->Add(new Button(grid1->CalculateCellRegion(5,5,5,5)));
    window->Add(new Button(grid1->CalculateCellRegion(6,6,6,6)));
    window->Add(new Button(grid1->CalculateCellRegion(7,7,8,8)));
    window->Add(new Button(grid1->CalculateCellRegion(8,8,8,8)));
    window->Add(new Button(grid1->CalculateCellRegion(9,9,9,9)));
    window->Add(new Button(grid1->CalculateCellRegion(10,10,10,10)));
    window->Add(new Button(grid1->CalculateCellRegion(11,11,11,11)));
    window->Add(new Button(grid1->CalculateCellRegion(12,12,12,12)));

    Grid*   grid2 = new Grid(elbo2->ContainerRegion(), rowh, gutter);
    window->Add(new Button(grid2->CalculateCellRegion(1,1, 1,6)));
    window->Add(new Button(grid2->CalculateCellRegion(1,1, 7,12)));

    Grid* grid3 = new Grid(elbo3->ContainerRegion(), rowh, gutter);

    window->Draw(renderer);

    SDL_RenderPresent(renderer);
  }

  return 0;
}
