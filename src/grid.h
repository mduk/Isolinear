#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "geometry.h"

class Grid {
  public:

    Grid(Size s)
      : bounds{Position{0,0}, s}
    {};

    Grid(Region b)
      : bounds{b}
    {};

    Grid(
        Region b,
        int rh,
        int nc,
        Margin m,
        Gutter g
      ) :
        bounds{b},
        row_height{rh},
        num_cols{nc},
        margin{m},
        gutter{g}
    {};

      Size CellSize();

    Region SingleCellRegion(int, int);
    Region  MultiCellRegion(int, int, int, int);

      Grid SubGrid(int, int, int, int);

    int PositionColumn(Position&);
    int PositionRow(Position&);
    Region PositionRegion(Position&);

    void DrawCells(SDL_Renderer*);

  protected:
    Region bounds;
    int num_cols{12};
    int row_height{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
};

