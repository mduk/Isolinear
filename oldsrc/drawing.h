#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Drawable {
  public:
    Region* bounds;

    virtual void Draw(
        SDL_Renderer*
      ) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent*
      ) = 0;

    Region* Bounds() const {
      return this->bounds;
    }
};
