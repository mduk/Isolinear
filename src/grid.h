#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"

class Grid {
  public:
    Region* where;
    int rowh;
    int gutter;

    Grid(
        Region* where,
        int rowh,
        int gutter
      );

    void Draw(SDL_Renderer* renderer);

    Region* CalculateLeftCapRegion(int row);
    Region* CalculateCellRegion(int row, int left_col, int right_col);
    Region* CalculateRightCapRegion(int row);

    void OnMouseButtonDown(SDL_MouseButtonEvent* event);

  protected:
    int num_cols = 12;
};
