#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "window.h"

using Column = int;
using Row = int;

class Grid {
  public:

    Grid(Region _b)
      : bounds{_b}
    {};

      Size CellSize();

    Region SingleCellRegion(int, int);
    Region MultiCellRegion(int, int, int, int);

    Column PositionColumn(Position&);
       Row PositionRow   (Position&);
    Region PositionRegion(Position&);

  protected:
    Region bounds;
    int num_cols = 12;
    int rowh = 100;
    int gutter = 10;
};

