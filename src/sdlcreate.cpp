#include "sdlcreate.h"

SDL_Window* createWindow(int w, int h) {
  SDL_Window *window = SDL_CreateWindow(
    "Isolinear",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    w, h,
    SDL_WINDOW_OPENGL // | SDL_WINDOW_FULLSCREEN_DESKTOP
  );

  if (window == NULL) {
    printf("SDL_CreateWindow failed: %s\n", SDL_GetError());
    exit(1);
  }

  return window;
}

SDL_Renderer* createRenderer(SDL_Window *window) {
  SDL_Renderer *renderer = SDL_CreateRenderer(
    window, -1, SDL_RENDERER_SOFTWARE
  );

  if (!renderer) {
    fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
    exit(1);
  }

  return renderer;
}
