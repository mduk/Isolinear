#include <stdio.h>
#include <sstream>

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
  curlpp::initialize();
  //cout << "Response: <" << http_req("http://ifconfig.co") << "> \n";

  SDL_Init(SDL_INIT_VIDEO);

  int win_h = 1250,
      win_w = win_h * 1.618;

  window = createWindow(win_w, win_h);
  renderer = createRenderer(window);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  Region* region = new Region(0, 0, win_w, win_h);
  Grid* grid = new Grid(region, 90, 5);
  //grid->Draw(renderer);

  int i, j;

  delete grid->CalculateCellRegion(1,  1, 12)->Draw(renderer);

  for (i=1; i<=12; i++) {
    delete grid->CalculateCellRegion(2, i, i)->Draw(renderer);
  }

  delete grid->CalculateCellRegion(3,  1,  2)->Draw(renderer);
  delete grid->CalculateCellRegion(3,  3,  4)->Draw(renderer);
  delete grid->CalculateCellRegion(3,  5,  6)->Draw(renderer);
  delete grid->CalculateCellRegion(3,  7,  8)->Draw(renderer);
  delete grid->CalculateCellRegion(3,  9, 10)->Draw(renderer);
  delete grid->CalculateCellRegion(3, 11, 12)->Draw(renderer);

  delete grid->CalculateCellRegion(4,  1,  3)->Draw(renderer);
  delete grid->CalculateCellRegion(4,  4,  6)->Draw(renderer);
  delete grid->CalculateCellRegion(4,  7,  9)->Draw(renderer);
  delete grid->CalculateCellRegion(4, 10, 12)->Draw(renderer);

  delete grid->CalculateCellRegion(5,  1,  4)->Draw(renderer);
  delete grid->CalculateCellRegion(5,  5,  8)->Draw(renderer);
  delete grid->CalculateCellRegion(5,  9, 12)->Draw(renderer);

  delete grid->CalculateCellRegion(6,  1,  6)->Draw(renderer);
  delete grid->CalculateCellRegion(6,  7, 12)->Draw(renderer);

  delete grid->CalculateCellRegion(7,  1, 12)->Draw(renderer);

  delete grid, region;

  SDL_RenderPresent(renderer);

  running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
          SDL_Point mouse;
          SDL_GetMouseState(&mouse.x, &mouse.y);
          grid->OnMouseButtonDown(&e.button);
          break;
        case SDL_QUIT:
          running = false;
          break;
      }
    }
  }

  return 0;
}
