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
      grid.CalculateGridRegion(1,1, 1,2).QuadrantArc(renderer, Compass::NORTHEAST, Colours().light);
      grid.CalculateGridRegion(2,1, 2,2).QuadrantArc(renderer, Compass::SOUTHEAST, Colours().light);
      grid.CalculateGridRegion(3,1, 3,2).QuadrantArc(renderer, Compass::SOUTHWEST, Colours().light);
      grid.CalculateGridRegion(4,1, 4,2).QuadrantArc(renderer, Compass::NORTHWEST, Colours().light);
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
          grid.MaxColumns(),1,
          grid.MaxColumns(),2
        );
      region.Fill(renderer, Colours().light);
    }

};
