#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"

class Grid {
  public:
    int ncols, nrows;
    int rowh, roww;
    int gutter;

    Grid(
        int ncols,
        int nrows,
        int rowh,
        int roww,
        int gutter
      );
    void Draw(
        SDL_Renderer* renderer,
        Position* position
      );
};

class GridRow {
  public:
    GridRow(Grid* _g) : grid{_g} {};
    void Draw(
        SDL_Renderer* renderer,
        Region* where
      );

  protected:
    Grid* grid;
};
