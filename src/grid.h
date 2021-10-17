#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "geometry.h"

class Grid;

class GridRegion : public Region {
  public:
    GridRegion(int x, int y, int w, int h)
      : Region{x, y, w, h}
    {};
};

class Grid {
  public:

    Grid()
    {}

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
    GridRegion PositionRegion(Position&) const;

    GridRegion SingleCellRegion(int, int) const;
    GridRegion MultiCellRegion(int, int, int, int) const;
    GridRegion Row(int) const;
    GridRegion Column(int) const;

    void DrawCells(SDL_Renderer*) const;

    Region bounds;
  protected:
    int num_cols{12};
    int row_height{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
};

