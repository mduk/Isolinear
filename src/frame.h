#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "drawable.h"
#include "pointerevent.h"
#include "grid.h"
#include "window.h"
#include "buttonbar.h"
#include "sweep.h"

extern bool drawdebug;

class CompassLayout : public Drawable {
  protected:
    Grid grid;
    Window& window;

         int north;
    Vector2D northeast;
         int east;
    Vector2D southeast;
         int south;
    Vector2D southwest;
         int west;
    Vector2D northwest;

  public:
    CompassLayout(
        Grid g, Window& win,
        int n, int e, int s, int w,
        Vector2D ne, Vector2D se,
        Vector2D sw, Vector2D nw
      ) :
        grid(g), window(win),
        north(n), east(e), south(s), west(w),
        northeast(ne), southeast(se),
        southwest(sw), northwest(nw)
    {}

    Grid North() const {
      int near_col = northwest.x + 1;
      int near_row = 1;
      int  far_col = grid.MaxColumns() - northeast.x;
      int  far_row = north;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid NorthEast() const {
      int near_col = grid.MaxColumns() - (northeast.x - 1);
      int near_row = 1;
      int  far_col = grid.MaxColumns();
      int  far_row = northeast.x;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid East() const {
      int near_col = grid.MaxColumns() - (east - 1);
      int near_row = northeast.y + 1;
      int  far_col = grid.MaxColumns();
      int  far_row = grid.MaxRows() - southeast.y;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid SouthEast() const {
      int near_col = grid.MaxColumns() - (southeast.x - 1);
      int near_row = grid.MaxRows() - (southeast.y - 1);
      int  far_col = grid.MaxColumns();;
      int  far_row = grid.MaxRows();

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid South() const {
      int near_col = southwest.x + 1;
      int near_row = grid.MaxRows() - (south - 1);
      int  far_col = grid.MaxColumns() - southeast.x;
      int  far_row = grid.MaxRows();

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid SouthWest() const {
      int near_col = 1;
      int near_row = grid.MaxRows() - (southwest.y - 1);
      int  far_col = southwest.x;
      int  far_row = grid.MaxRows();

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid West() const {
      int near_col = 1;
      int near_row = northwest.y + 1;
      int  far_col = west;
      int  far_row = grid.MaxRows() - southwest.y;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid NorthWest() const {
      int near_col = 1;
      int near_row = 1;
      int  far_col = northwest.x;
      int  far_row = northwest.y;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Grid Centre() const {
      int near_col = northwest.x + 1;
      int near_row = northwest.y + 1;
      int  far_col = grid.MaxColumns() - southeast.x;
      int  far_row = grid.MaxRows() - southeast.y;

      return grid.SubGrid(
          near_col, near_row,
           far_col, far_row
        );
    }

    Region2D Bounds() const
    {
      return grid.bounds;
    }

    virtual void Draw(SDL_Renderer* renderer) const override
    {
      if (north > 0) {
        North().bounds.Fill(renderer, 0x88888888);
      }

      if (northeast.x > 0 && northeast.y > 0) {
        NorthEast().bounds.Fill(renderer, 0x88888888);
      }

      if (east > 0) {
        East().bounds.Fill(renderer, 0x88888888);
      }

      if (southeast.x > 0 && southeast.y > 0) {
        SouthEast().bounds.Fill(renderer, 0x88888888);
      }

      if (south > 0) {
        South().bounds.Fill(renderer, 0x88888888);
      }

      if (southwest.x > 0 && southwest.y > 0) {
        SouthWest().bounds.Fill(renderer, 0x88888888);
      }

      if (west > 0) {
        West().bounds.Fill(renderer, 0x88888888);
      }

      if (northwest.x > 0 && northwest.y > 0) {
        NorthWest().bounds.Fill(renderer, 0x88888888);
      }

      Centre().bounds.Fill(renderer, 0x88888888);
    }
};

class Frame : public Drawable {
  protected:
    CompassLayout layout;
    Grid grid;
    Window& window;

    int outer_radius{90};
    int inner_radius{50};

    int north_frame = 2;
    int east_frame  = 2;
    int south_frame = 2;
    int west_frame  = 2;

    HorizontalButtonBar north_bar;
         NorthEastSweep northeast_sweep;
      VerticalButtonBar east_bar;
         SouthEastSweep southeast_sweep;
    HorizontalButtonBar south_bar;
         SouthWestSweep southwest_sweep;
      VerticalButtonBar west_bar;
         NorthWestSweep northwest_sweep;

  public:
    Frame(Grid g, Window& win, int n, int e, int s, int w)
      :
        layout{g, w, n, e, s, w, {e+1,n}, {e+1,s}, {w+1,n}, {w+1,s}},
        grid{g},
        window{win},

        north_bar{window, layout.North()},
        east_bar{window, layout.East()},
        south_bar{window, layout.South()},
        west_bar{window, layout.West()},

        northeast_sweep{window, layout.NorthEast(), Vector2D{ e+1, n }, outer_radius, inner_radius },
        southeast_sweep{window, layout.SouthEast(), Vector2D{ e+1, s }, outer_radius, inner_radius },
        southwest_sweep{window, layout.SouthWest(), Vector2D{ w+1, s }, outer_radius, inner_radius },
        northwest_sweep{window, layout.NorthWest(), Vector2D{ w+1, n }, outer_radius, inner_radius }
    {
      //
    }

    HorizontalButtonBar& NorthBar() { return north_bar; }
    VerticalButtonBar&   EastBar()  { return  east_bar; }
    HorizontalButtonBar& SouthBar() { return south_bar; }
    VerticalButtonBar&   WestBar()  { return  west_bar; }

    Region2D Bounds() const override {
      return grid.bounds;
    }

    virtual ColourScheme Colours() const override {
      return Drawable::Colours();
    }

    virtual void Colours(ColourScheme cs) override {
      Drawable::Colours(cs);
            north_bar.Colours(cs);
      northeast_sweep.Colours(cs);
             east_bar.Colours(cs);
      southeast_sweep.Colours(cs);
            south_bar.Colours(cs);
      southwest_sweep.Colours(cs);
             west_bar.Colours(cs);
      northwest_sweep.Colours(cs);
    }

    void Draw(SDL_Renderer* renderer) const override {
      if (north_frame > 0) {
        north_bar.Draw(renderer);
      }

      if (east_frame > 0) {
        east_bar.Draw(renderer);
      }

      if (south_frame > 0) {
        south_bar.Draw(renderer);
      }

      if (west_frame > 0) {
        west_bar.Draw(renderer);
      }
/*
      northeast_sweep.Draw(renderer);
      southeast_sweep.Draw(renderer);
      southwest_sweep.Draw(renderer);
      northwest_sweep.Draw(renderer);
*/    }
};
