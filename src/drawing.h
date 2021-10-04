#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Drawable {
  public:
    virtual void Draw(
        SDL_Renderer* renderer
      ) const = 0;

    virtual Region* GetBounds() const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent* e
      ) = 0;
};
