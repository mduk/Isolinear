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

  int margin = 20;
  Position* p = new Position(margin, margin);
  Grid* g = new Grid(6, 90, (win_w - (margin * 2)), 5);
  g->Draw(renderer, p);
  delete p, g;

  SDL_RenderPresent(renderer);

  running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_MOUSEBUTTONDOWN:
          SDL_Point mouse;
          SDL_GetMouseState(&mouse.x, &mouse.y);
          g->OnMouseButtonDown(&e.button);
          break;
        case SDL_QUIT:
          running = false;
          break;
      }
    }
  }

  return 0;
}
