#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Drawable {
  public:
    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) = 0;

    virtual SDL_Rect SdlRect() const = 0;
};
