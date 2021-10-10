#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"

using namespace std;

class Drawable {
  public:
    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) = 0;

    virtual Region Bounds() = 0;
};

class Window {
  public:
    Size size;

    Window(int, int);
    ~Window() {
      for (auto* drawable : drawables) {
        delete drawable;
      }
    }

    void Add(Drawable*);
    void Draw();
    void OnMouseButtonDown(SDL_MouseButtonEvent&);

    SDL_Renderer* sdl_renderer;
  protected:
    SDL_Window* sdl_window;
    std::list<Drawable*> drawables;
};
