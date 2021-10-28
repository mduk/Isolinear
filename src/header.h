#pragma once

#include "geometry.h"

class Header : public Region2D {
  protected:
    Grid grid;

  public:
    Header(Grid g)
      : grid{g}
    {};

    void Draw(SDL_Renderer* renderer) const override {
      int x = 1,
          y = 1,
          w = 8,
          h = 16;

      grid.CalculateGridRegion(  x,     y,   x ,    y+1).QuadrantArc(renderer, Compass::NORTHWEST, Colours().light);
      grid.CalculateGridRegion(w+x,     y, w+x ,    y+1).QuadrantArc(renderer, Compass::NORTHEAST, Colours().light);
      grid.CalculateGridRegion(  x,   h+y,   x ,  h+y+1).QuadrantArc(renderer, Compass::SOUTHWEST, Colours().light);
      grid.CalculateGridRegion(w+x,   h+y, w+x ,  h+y+1).QuadrantArc(renderer, Compass::SOUTHEAST, Colours().light);

      grid.CalculateGridRegion(  x+1,   y, w+x-1,   y+1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(  x+1, h+y, w+x-1, h+y+1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(  x  , 2+y,   x  , h+y-1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(w+x  , 2+y, w+x  , h+y-1).Fill(renderer, Colours().dark);
    }


    void DrawLeftCap(SDL_Renderer* renderer) const {
      Region2D region = grid.CalculateGridRegion(
          1,1,
          1,2
        );
      region.QuadrantArc(renderer, Compass::SOUTHWEST, Colours().light);
    }

    void DrawRightCap(SDL_Renderer* renderer) const {
      Region2D region = grid.CalculateGridRegion(
          1,1,
          12,2
        );
      region.Fill(renderer, Colours().light);
    }

};
