#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

class Drawable {
  public:
    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) {};

    virtual SDL_Rect SdlRect() const = 0;
};

