#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "sdlcreate.h"
#include "geometry.h"
#include "button.h"
#include "grid.h"

bool running = false;
SDL_Window *window;
SDL_Renderer *renderer;

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);

  int win_h = 999,
      win_w = win_h * 1.618;

  window = createWindow(win_w, win_h);
  renderer = createRenderer(window);

  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  int margin = 20;
  Position* p = new Position(margin, margin);
  Grid* g = new Grid(10, 6, 90, (win_w - (margin * 2)), 5);
  g->Draw(renderer, p);
  delete p, g;

  SDL_RenderPresent(renderer);

  running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {
        case SDL_QUIT: running = false; break;
      }
    }
  }

  return 0;
}
