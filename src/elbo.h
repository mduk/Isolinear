#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"

class Elbo {
  public:
    Elbo(
        Region* _b,
        Position* _c,
        Colour _col
      ) :
        bounds{_b},
        corner{_c},
        colour{_col}
    { };
    void Draw(
        SDL_Renderer* renderer
      );

    Region* ContainerRegion();

  protected:
    Region* bounds;
    Position* corner;
    Colour colour;

    int outer_radius =  90;
    int inner_radius = 60;
    int gutter = 20;
};
