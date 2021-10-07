#pragma once

#include <exception>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

using namespace std;

class Window {
  public:
    Window(int w, int h);

  protected:
    int width_px;
    int height_px;
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
};
