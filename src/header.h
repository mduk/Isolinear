#pragma once

#include "geometry.h"

class Header : public Drawable {
  protected:
    Grid grid;

  public:
    Header(Grid g)
      : grid{g}
    {};

    void Draw(SDL_Renderer* renderer) const override {
      grid.CalculateGridRegion(2,1, grid.MaxColumns()-1,1)
        .Fill(renderer, colours.dark);

      DrawLeftCap(renderer);
      DrawRightCap(renderer);
    }

    void DrawLeftCap(SDL_Renderer* renderer) const {
      Region2D region = grid.CalculateGridRegion(
          1,1,
          1,2
        );
      region.Fill(renderer, colours.light);
    }

    void DrawRightCap(SDL_Renderer* renderer) const {
      Region2D region = grid.CalculateGridRegion(
          grid.MaxColumns(),1,
          grid.MaxColumns(),2
        );
      region.Fill(renderer, colours.light_alternate);
    }

};
