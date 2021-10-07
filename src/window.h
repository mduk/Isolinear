#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

using namespace std;

class Drawable {
  public:
    Region bounds;

    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent*
      ) = 0;
};

class Window {
  public:
    Window(int w, int h);

    void Draw();

  protected:
    int width_px;
    int height_px;
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    std::list<Drawable*> drawables;
};
