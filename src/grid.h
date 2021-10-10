#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "window.h"

using NumCols = int;
using Column = int;
using Row = int;

class Grid {
  public:

    Grid(Size _s)
      : bounds{Position{0,0},_s}
    {};

    Grid(Region _b)
      : bounds{_b}
    {};

    Grid(Region _b, NumCols _nc)
      : bounds{_b},
        num_cols(_nc)
    {};

      Size CellSize();

    Region SingleCellRegion(int, int);
    Region MultiCellRegion(int, int, int, int);

    Column PositionColumn(Position&);
       Row PositionRow   (Position&);
    Region PositionRegion(Position&);

  protected:
    Region bounds;
    NumCols num_cols = 12;
    int rowh = 100;
    int gutter = 10;
};

