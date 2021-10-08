#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "window.h"

class Grid {
  public:

    Grid(Region _b)
      : bounds{_b}
    {};

    Region CalculateRegion(int, int, int, int);

  protected:
    Region bounds;
    int num_cols = 12;
    int rowh = 100;
    int gutter = 10;
};

