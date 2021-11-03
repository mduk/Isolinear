#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

class Drawable {
  protected:
    ColourScheme colours;

  public:
    virtual SDL_Rect SdlRect() const = 0;
    virtual void Draw(SDL_Renderer*) const = 0;

    virtual void OnMouseButtonDown(
        SDL_MouseButtonEvent&
      ) {};

    virtual void OnPointerEvent(PointerEvent e) {};

    virtual ColourScheme Colours() const {
      return colours;
    }

    virtual void Colours(ColourScheme cs) {
      colours = cs;
    }
};

