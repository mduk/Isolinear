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

    Size CellSize() const;
    Grid SubGrid(int, int, int, int) const;

    int PositionColumn(Position&) const;
    int PositionRow(Position&) const;
    Region PositionRegion(Position&) const;

    Region SingleCellRegion(int, int) const;
    Region MultiCellRegion(int, int, int, int) const;
    Region Row(int) const;
    Region Column(int) const;

    void DrawCells(SDL_Renderer*) const;

    Region bounds;
  protected:
    int num_cols{12};
    int row_height{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
};

