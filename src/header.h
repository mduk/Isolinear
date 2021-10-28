#pragma once

#include "geometry.h"

class Header : public Region2D {
  protected:
    Grid grid;
    Window& window;
    bool header_bar{true};
    bool footer_bar{true};

  public:
    Header(Grid g, Window& w)
      : grid{g}, window{w}
    {};

    void Draw(SDL_Renderer* renderer) const override {
      int x = 1,
          y = 1,
          w = grid.MaxColumns() - 1,
          h = grid.MaxRows() - 2;

      if (header_bar) {
        Region2D  left_cap = grid.CalculateGridRegion(  x  ,   y  ,   x  ,   y+1);
        Region2D right_cap = grid.CalculateGridRegion(w+x  ,   y  , w+x  ,   y+1);
        Region2D       bar = grid.CalculateGridRegion(  x+1,   y  , w+x-1,   y+1);

         left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
        right_cap.Bullnose(renderer, Compass::EAST, Colours().light);
              bar.Fill    (renderer, Colours().dark);

        std::string text{"WINDOW TITLE HERE "};
        RenderedText headertext = window.HeaderFont().RenderText(Colours().background, text);
        headertext.Draw(renderer, Compass::EAST, bar);
      }

      if (footer_bar) {
        Region2D  left_cap = grid.CalculateGridRegion(  x  , h+y  ,   x  , h+y+1);
        Region2D right_cap = grid.CalculateGridRegion(w+x  , h+y  , w+x  , h+y+1);
        Region2D       bar = grid.CalculateGridRegion(  x+1, h+y  , w+x-1, h+y+1);

         left_cap.Bullnose(renderer, Compass::WEST, Colours().light);
        right_cap.Bullnose(renderer, Compass::EAST, Colours().light);
              bar.Fill    (renderer, Colours().dark);
      }


      return;

      grid.CalculateGridRegion(  x  ,   y  ,   x  ,   y+1).QuadrantArc(renderer, Compass::NORTHWEST, Colours().light);
      grid.CalculateGridRegion(w+x  ,   y  , w+x  ,   y+1).QuadrantArc(renderer, Compass::NORTHEAST, Colours().light);
      grid.CalculateGridRegion(  x  , h+y  ,   x  , h+y+1).QuadrantArc(renderer, Compass::SOUTHWEST, Colours().light);
      grid.CalculateGridRegion(w+x  , h+y  , w+x  , h+y+1).QuadrantArc(renderer, Compass::SOUTHEAST, Colours().light);

      grid.CalculateGridRegion(  x+1,   y  , w+x-1,   y+1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(  x+1, h+y  , w+x-1, h+y+1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(  x  ,   y+2,   x  , h+y-1).Fill(renderer, Colours().dark);
      grid.CalculateGridRegion(w+x  ,   y+2, w+x  , h+y-1).Fill(renderer, Colours().dark);

      grid.CalculateGridRegion( 4,4, 4,5 ).Bullnose(renderer, Compass::WEST,  Colours().light_alternate);
      grid.CalculateGridRegion( 5,4, 5,5 ).Bullnose(renderer, Compass::SOUTH, Colours().light_alternate);
      grid.CalculateGridRegion( 6,4, 6,5 ).Bullnose(renderer, Compass::NORTH, Colours().light_alternate);
      grid.CalculateGridRegion( 7,4, 7,5 ).Bullnose(renderer, Compass::EAST,  Colours().light_alternate);
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
