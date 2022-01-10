#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "drawable.h"
#include "pointerevent.h"
#include "grid.h"
#include "display.h"


extern bool drawdebug;


namespace isolinear {


  class CompassLayout : public ui::drawable {
    protected:
      isolinear::grid grid;
      display::window& window;

      int north;
      int east;
      int south;
      int west;
      geometry::vector northeast;
      geometry::vector southeast;
      geometry::vector southwest;
      geometry::vector northwest;

    public:
      CompassLayout(
          isolinear::grid g, display::window& win,
          int n, int e, int s, int w,
          geometry::vector ne, geometry::vector se,
          geometry::vector sw, geometry::vector nw
        ) :
          grid(g), window(win),
          north(n), east(e), south(s), west(w),
          northeast(ne), southeast(se),
          southwest(sw), northwest(nw)
      {}

      isolinear::grid North() const {
        int near_col = northwest.x + 1;
        int near_row = 1;
        int  far_col = grid.MaxColumns() - northeast.x;
        int  far_row = north;

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid NorthEast() const {
        int near_col = grid.MaxColumns() - (northeast.x - 1);
        int near_row = 1;
        int  far_col = grid.MaxColumns();
        int  far_row = northeast.x;

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid East() const {
        int near_col = grid.MaxColumns() - (east - 1);
        int near_row = northeast.y + 1;
        int  far_col = grid.MaxColumns();
        int  far_row = grid.MaxRows() - southeast.y;

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid SouthEast() const {
        int near_col = grid.MaxColumns() - (southeast.x - 1);
        int near_row = grid.MaxRows() - (southeast.y - 1);
        int  far_col = grid.MaxColumns();;
        int  far_row = grid.MaxRows();

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid South() const {
        int near_col = southwest.x + 1;
        int near_row = grid.MaxRows() - (south - 1);
        int  far_col = grid.MaxColumns() - southeast.x;
        int  far_row = grid.MaxRows();

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid SouthWest() const {
        int near_col = 1;
        int near_row = grid.MaxRows() - (southwest.y - 1);
        int  far_col = southwest.x;
        int  far_row = grid.MaxRows();

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid West() const {
        int near_col = 1;
        int near_row = northwest.y + 1;
        int  far_col = west;
        int  far_row = grid.MaxRows() - southwest.y;

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid NorthWest() const {
        int near_col = 1;
        int near_row = 1;
        int  far_col = northwest.x;
        int  far_row = northwest.y;

        return grid.SubGrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid Centre() const {
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


}
