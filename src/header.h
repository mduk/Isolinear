#pragma once

#include "geometry.h"

class Header : public Region2D {
  protected:
    Grid grid;
    Window& window;
    std::string text{""};

  public:
    Header(Grid g, Window& w, std::string t)
      : grid{g}, window{w}, text{t}
    {};

    Header(Grid g, Window& w)
      : grid{g}, window{w}
    {};

    void Draw(SDL_Renderer* renderer) const override {
      int x = 1,
          y = 1,
          w = grid.MaxColumns() - 1,
          h = grid.MaxRows() - 2;

      Region2D  left_cap = grid.CalculateGridRegion(  x  ,   y  ,   x  ,   y+1);
      Region2D right_cap = grid.CalculateGridRegion(w+x  ,   y  , w+x  ,   y+1);
      Region2D       bar = grid.CalculateGridRegion(  x+1,   y  , w+x-1,   y+1);

       left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
      right_cap.Bullnose(renderer, Compass::EAST, Colours().light);

      if (text.length() == 0) {
        bar.Fill(renderer, Colours().dark);
        return;
      }

      RenderedText headertext = window.HeaderFont().RenderText(Colours().active, text);
      Region2D headerregion = bar.Align(Compass::EAST, headertext.Size());

      int col = grid.PositionColumnIndex(headerregion.Near());
      Region2D cell = grid.CalculateGridRegion(col, y, col, y+1);
      Region2D fillerregion{
          cell.Origin(),
          Position2D{
              headerregion.SouthWestX(),
              cell.FarY()
            }
        };

      grid.CalculateGridRegion(x+1, y, col-1, y+1).Fill(renderer, Colours().dark);
      fillerregion.Fill(renderer, Colours().light);

      headertext.Draw(renderer, Compass::EAST, bar);
    }
};
