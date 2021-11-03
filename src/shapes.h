#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "geometry.h"


class Quad : public Drawable {
  public:
    Quad(Region2D& r) :
        region{r} {};

    void Draw(SDL_Renderer* renderer) const {
      boxColor(renderer,
          region.NearX(), region.NearY(),
          region.FarX(), region.FarY(),
          Colours().frame
        );
    }

  protected:
    Region2D& region;
};
