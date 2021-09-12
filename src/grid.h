#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"

class Grid {
  public:
    Grid(
        int ncols,
        int nrows,
        int rowh
      );
    void Draw(
        SDL_Renderer* renderer,
        Position* position
      );

  private:
    int ncols, nrows;
    int rowh;
};
