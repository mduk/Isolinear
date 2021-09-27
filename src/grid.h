#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"

class GridAssignment {
  public:
    Region* region;
    Button* button;
    GridAssignment(Region* _r, Button* _b) : region{_r}, button{_b} {}
};

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
    Region* CalculateCellRegion(
        int top_row, int bottom_row,
        int left_col, int right_col
      );
    Region* CalculateRightCapRegion(int row);
    Region* AssignRegion(
        int top_row, int bottom_row,
        int left_col, int right_col,
        Button*
      );
    Region* AssignRegion(
        Region* region,
        Button* button
      );

    void OnMouseButtonDown(SDL_MouseButtonEvent* event);

  protected:
    int num_cols = 12;
    std::list<GridAssignment*> assigned_regions;
};

