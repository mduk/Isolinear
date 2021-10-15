#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "geometry.h"

using NumCols = int;
using Column = int;
using Row = int;
using RowHeight = int;

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
        RowHeight rh,
        NumCols nc,
        Margin m,
        Gutter g
      ) :
        bounds{b},
        rowh{rh},
        num_cols{nc},
        margin{m},
        gutter{g}
    {};

      Size CellSize();

    Region SingleCellRegion(int, int);
    Region  MultiCellRegion(int, int, int, int);

    Column PositionColumn(Position&);
       Row PositionRow   (Position&);
    Region PositionRegion(Position&);

    void DrawCells(SDL_Renderer*);

  protected:
    Region bounds;
    NumCols num_cols{12};
    int rowh{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
};

