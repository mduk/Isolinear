#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "drawable.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"


using namespace std;


class Window {
  public:
    Size size;

    Window(int, int);
    ~Window() {
      for (auto* drawable : drawables) {
        delete drawable;
      }

      SDL_DestroyRenderer(sdl_renderer);
      SDL_DestroyWindow(sdl_window);
    }

    void Add(Drawable*);
    void Draw();
    void OnMouseButtonDown(SDL_MouseButtonEvent&);

  protected:
    SDL_Window* sdl_window;
    SDL_Renderer* sdl_renderer;
    std::list<Drawable*> drawables;
};
