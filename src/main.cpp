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
  Elbo* elbo1 = new Elbo(grid->CalculateCellRegion( 1,14, 1, 6), elbo_corner, 0xff3399ff);
  Elbo* elbo2 = new Elbo(grid->CalculateCellRegion( 7,14, 7,12), elbo_corner, 0xff3399ff);
  Elbo* elbo3 = new Elbo(grid->CalculateCellRegion( 1, 6, 7,12), elbo_corner, 0xff3399ff);

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

    int rowh = 100;
    int gutter = 10;

    Grid*   grid1    = new Grid(elbo1->ContainerRegion(), rowh, gutter);
    Button* button1  = new Button(grid1->CalculateCellRegion(1,1,1,2));
    Button* button2  = new Button(grid1->CalculateCellRegion(2,2,2,2));
    Button* button3  = new Button(grid1->CalculateCellRegion(3,3,3,3));
    Button* button4  = new Button(grid1->CalculateCellRegion(4,4,4,4));
    Button* button5  = new Button(grid1->CalculateCellRegion(5,5,5,5));
    Button* button6  = new Button(grid1->CalculateCellRegion(6,6,6,6));
    Button* button7  = new Button(grid1->CalculateCellRegion(7,7,8,8));
    Button* button8  = new Button(grid1->CalculateCellRegion(8,8,8,8));
    Button* button9  = new Button(grid1->CalculateCellRegion(9,9,9,9));
    Button* button10 = new Button(grid1->CalculateCellRegion(10,10,10,10));
    Button* button11 = new Button(grid1->CalculateCellRegion(11,11,11,11));
    Button* button12 = new Button(grid1->CalculateCellRegion(12,12,12,12));

    Grid*   grid2 = new Grid(elbo2->ContainerRegion(), rowh, gutter);
    Button* button13 = new Button(grid2->CalculateCellRegion(1,1, 1,6));
    Button* button14 = new Button(grid2->CalculateCellRegion(1,1, 7,12));

    Grid* grid3 = new Grid(elbo3->ContainerRegion(), rowh, gutter);

    grid->Draw(renderer);

    elbo1->Draw(renderer);
    elbo2->Draw(renderer);
    elbo3->Draw(renderer);

    grid1->Draw(renderer);
    grid2->Draw(renderer);
    grid3->Draw(renderer);

    button1->Draw(renderer);
    button2->Draw(renderer);
    button3->Draw(renderer);
    button4->Draw(renderer);
    button5->Draw(renderer);
    button6->Draw(renderer);
    button7->Draw(renderer);
    button8->Draw(renderer);
    button9->Draw(renderer);
    button10->Draw(renderer);
    button11->Draw(renderer);
    button12->Draw(renderer);
    button13->Draw(renderer);
    button14->Draw(renderer);

    SDL_RenderPresent(renderer);
  }

  delete grid;
  delete elbo1, elbo2, elbo3;
  delete elbo_corner;

  return 0;
}
