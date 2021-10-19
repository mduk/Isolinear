#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "geometry.h"


class Quad : public Drawable {
  public:
    Quad(Region&& r, uint32_t fc) :
        fill_colour{fc}
    {
      region = r;
    };

    void Draw(SDL_Renderer* renderer) const {
      printf("Quad::Draw %d,%d %d,%d\n",
          region.NearX(), region.NearY(),
          region.FarX(), region.FarY()
        );
      boxColor(renderer,
          region.NearX(), region.NearY(),
          region.FarX(), region.FarY(),
          fill_colour
        );
    }

    SDL_Rect SdlRect() const {
      return region.SdlRect();
    }

  protected:
    Region region;
    uint32_t fill_colour;
};
