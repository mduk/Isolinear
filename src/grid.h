#pragma once

#include <list>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "button.h"

typedef Uint32 Colour;
Colour RandomColour();

class GridCell {
  public:
    int width;
    Colour colour;
    Button* button;

    GridCell(Button* _b, int _w) : button{_b}, width{_w} {};
    void Draw(SDL_Renderer* renderer, Region* where);
};

class Grid {
  public:
    int nrows;
    int rowh, roww;
    int gutter;

    Grid(
        int nrows,
        int rowh,
        int roww,
        int gutter
      );
    void Draw(
        SDL_Renderer* renderer,
        Position* position
      );
};

class GridRow {
  public:
    GridRow(Grid* _g) : grid{_g} {};
    void Draw(
        SDL_Renderer* renderer,
        Region* where
      );

  protected:
    Grid* grid;
    std::list<GridCell*> cells;
};

