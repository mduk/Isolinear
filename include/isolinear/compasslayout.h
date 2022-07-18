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


namespace isolinear::layout {

  class compass : public ui::drawable {
    protected:
      isolinear::grid m_grid;
      display::window& m_window;

      int m_north;
      int m_east;
      int m_south;
      int m_west;
      geometry::vector m_northeast;
      geometry::vector m_southeast;
      geometry::vector m_southwest;
      geometry::vector m_northwest;

    public:
      compass(
          isolinear::grid g, display::window& win,
          int n, int e, int s, int w,
          geometry::vector ne, geometry::vector se,
          geometry::vector sw, geometry::vector nw
        ) :
          m_grid(g), m_window(win),
          m_north(n), m_east(e), m_south(s), m_west(w),
          m_northeast(ne), m_southeast(se),
          m_southwest(sw), m_northwest(nw)
      {}

      isolinear::grid North() const {
        int near_col = m_northwest.x + 1;
        int near_row = 1;
        int  far_col = m_grid.max_columns() - m_northeast.x;
        int  far_row = m_north;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid NorthEast() const {
        int near_col = m_grid.max_columns() - (m_northeast.x - 1);
        int near_row = 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_northeast.x;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid East() const {
        int near_col = m_grid.max_columns() - (m_east - 1);
        int near_row = m_northeast.y + 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_grid.max_rows() - m_southeast.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid SouthEast() const {
        int near_col = m_grid.max_columns() - (m_southeast.x - 1);
        int near_row = m_grid.max_rows() - (m_southeast.y - 1);
        int  far_col = m_grid.max_columns();;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid South() const {
        int near_col = m_southwest.x + 1;
        int near_row = m_grid.max_rows() - (m_south - 1);
        int  far_col = m_grid.max_columns() - m_southeast.x;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid SouthWest() const {
        int near_col = 1;
        int near_row = m_grid.max_rows() - (m_southwest.y - 1);
        int  far_col = m_southwest.x;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid West() const {
        int near_col = 1;
        int near_row = m_northwest.y + 1;
        int  far_col = m_west;
        int  far_row = m_grid.max_rows() - m_southwest.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid NorthWest() const {
        int near_col = 1;
        int near_row = 1;
        int  far_col = m_northwest.x;
        int  far_row = m_northwest.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      isolinear::grid Centre() const {
        int near_col = m_northwest.x + 1;
        int near_row = m_northwest.y + 1;
        int  far_col = m_grid.max_columns() - m_southeast.x;
        int  far_row = m_grid.max_rows() - m_southeast.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      geometry::region bounds() const {
        return m_grid.bounds();
      }

      virtual void draw(SDL_Renderer* renderer) const override {
        if (m_north > 0) {
          North().bounds().draw(renderer);
        }

        if (m_northeast.x > 0 && m_northeast.y > 0) {
          NorthEast().bounds().draw(renderer);
        }

        if (m_east > 0) {
          East().bounds().draw(renderer);
        }

        if (m_southeast.x > 0 && m_southeast.y > 0) {
          SouthEast().bounds().draw(renderer);
        }

        if (m_south > 0) {
          South().bounds().draw(renderer);
        }

        if (m_southwest.x > 0 && m_southwest.y > 0) {
          SouthWest().bounds().draw(renderer);
        }

        if (m_west > 0) {
          West().bounds().draw(renderer);
        }

        if (m_northwest.x > 0 && m_northwest.y > 0) {
          NorthWest().bounds().draw(renderer);
        }

        Centre().bounds().draw(renderer);
      }
  };


}
