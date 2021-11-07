#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"
#include "pointerevent.h"

class Drawable {
  protected:
    Region2D bounds;
    ColourScheme colours;

  public:
    virtual void Draw(SDL_Renderer*) const = 0;
    virtual Region2D Bounds() const = 0;

    virtual void Update() {}

    virtual void OnPointerEvent(PointerEvent event) {
      Region2D   b = Bounds();
      Position2D p = event.Position();

      printf("Region: %d,%d (%d,%d) %d,%d \n",
          b.NearX(), b.NearY(),
          b.W(),     b.H(),
          b.FarX(),  b.FarY()
        );
      printf(" Click: %d,%d (%d,%d local)\n",
          p.x, p.y,
          p.x - b.NearX(),
          p.y - b.NearY()
        );
    }

    virtual ColourScheme Colours() const {
      return colours;
    }

    virtual void Colours(ColourScheme cs) {
      colours = cs;
    }
};

