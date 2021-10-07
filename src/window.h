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
    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent*
      ) = 0;
};

class Window {
  public:
    Window(int, int);

    void Add(Drawable*);
    void Draw();

  protected:
    int width_px;
    int height_px;
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;

    std::list<Drawable*> drawables;
};

class Button : public Drawable {
  public:
    void Draw(SDL_Renderer*) const;
    void OnMouseButtonDown(SDL_MouseButtonEvent*);
    Region Bounds();
};
