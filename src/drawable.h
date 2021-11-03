#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

class Drawable {
  protected:
    Region2D bounds;
    ColourScheme colours;

  public:
    virtual void Draw(SDL_Renderer*) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) {};

    virtual void OnPointerEvent(PointerEvent e) {};

    virtual Region2D Bounds() const {
      return bounds;
    }

    virtual ColourScheme Colours() const {
      return colours;
    }

    virtual void Colours(ColourScheme cs) {
      colours = cs;
    }
};

