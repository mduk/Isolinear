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

      printf("North : %d,%d %d,%d\n",
          near_col, near_row,
           far_col, far_row
        );

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
        North().bounds.Draw(renderer);
      }

      if (northeast.x > 0 && northeast.y > 0) {
        NorthEast().bounds.Draw(renderer);
      }

      if (east > 0) {
        East().bounds.Draw(renderer);
      }

      if (southeast.x > 0 && southeast.y > 0) {
        SouthEast().bounds.Draw(renderer);
      }

      if (south > 0) {
        South().bounds.Draw(renderer);
      }

      if (southwest.x > 0 && southwest.y > 0) {
        SouthWest().bounds.Draw(renderer);
      }

      if (west > 0) {
        West().bounds.Draw(renderer);
      }

      if (northwest.x > 0 && northwest.y > 0) {
        NorthWest().bounds.Draw(renderer);
      }

      Centre().bounds.Draw(renderer);
    }
};

class Frame : public Drawable {
  protected:
    CompassLayout layout;
    Grid grid;
    Window& window;

    int outer_radius{90};
    int inner_radius{50};

    int n_size{0};
    int e_size{0};
    int s_size{0};
    int w_size{0};

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
        n_size{n}, e_size{e}, s_size{s}, w_size{w},
        layout{ g, win,
                n, e, s, w,
                { e ? e+1 : e, n ? n+1 : n },
                { e ? e+1 : e, s ? s+1 : s },
                { w ? w+1 : w, n ? n+1 : n },
                { w ? w+1 : w, s ? s+1 : s }
              },
        grid{g},
        window{win},

        north_bar( window, layout.North() ),
        east_bar( window, layout.East() ),
        south_bar( window, layout.South() ),
        west_bar( window, layout.West() ),

        northeast_sweep( window, layout.NorthEast(), Vector2D( e, n ), outer_radius, inner_radius ),
        southeast_sweep( window, layout.SouthEast(), Vector2D( e, s ), outer_radius, inner_radius ),
        southwest_sweep( window, layout.SouthWest(), Vector2D( w, s ), outer_radius, inner_radius ),
        northwest_sweep( window, layout.NorthWest(), Vector2D( w, n ), outer_radius, inner_radius )
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

    virtual void Draw(SDL_Renderer* renderer) const override {
      if (n_size > 0) {
        north_bar.Draw(renderer);
      }
      if (e_size > 0) {
        east_bar.Draw(renderer);
      }
      if (s_size > 0) {
        south_bar.Draw(renderer);
      }
      if (w_size > 0) {
        west_bar.Draw(renderer);
      }

      if (n_size > 0 && e_size > 0) {
        northeast_sweep.Draw(renderer);
      }
      if (s_size > 0 && e_size > 0) {
        southeast_sweep.Draw(renderer);
      }
      if (s_size > 0 && w_size > 0) {
        southwest_sweep.Draw(renderer);
      }
      if (n_size > 0 && w_size > 0) {
        northwest_sweep.Draw(renderer);
      }

      if (drawdebug) {
        layout.Draw(renderer);
      }
    }
};
