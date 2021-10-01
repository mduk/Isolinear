#pragma once

#include <list>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"
#include "colours.h"
#include "drawing.h"

class GridAssignment {
  public:
    Region* region;
    Drawable* drawable;
    GridAssignment(Region* _r, Drawable* _d)
      : region{_r}, drawable{_d} {}
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

    Region* CalculateCellRegion(
        int top_row, int bottom_row,
        int left_col, int right_col
      );
    Region* AssignRegion(
        int top_row, int bottom_row,
        int left_col, int right_col,
        Button*
      );
    Region* AssignRegion(
        Region* region,
        Drawable* drawable
      );

    void OnMouseButtonDown(
        SDL_MouseButtonEvent* event
      );

  protected:
    int num_cols = 12;
    std::list<GridAssignment*> assigned_regions;
};

