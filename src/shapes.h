#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "geometry.h"


class Quad : public Drawable {
  protected:
    Region2D& bounds;

  public:
    Quad(Region2D& r) :
        bounds{r} {};

    Region2D Bounds() const override {
      return bounds;
    }

    void Draw(SDL_Renderer* renderer) const override {
      boxColor(renderer,
          bounds.NearX(), bounds.NearY(),
          bounds.FarX(), bounds.FarY(),
          Colours().frame
        );
    }

};
