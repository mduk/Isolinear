#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"
#include "control.h"
#include "event.h"
#include "grid.h"
#include "display.h"


namespace isolinear::layout {

  class compass : public ui::control {
    protected:
      grid& m_grid;
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
          grid& g, display::window& win,
          int n, int e, int s, int w,
          geometry::vector ne, geometry::vector se,
          geometry::vector sw, geometry::vector nw
        ) :
          m_grid(g), m_window(win),
          m_north(n), m_east(e), m_south(s), m_west(w),
          m_northeast(ne), m_southeast(se),
          m_southwest(sw), m_northwest(nw)
      {}

      compass( grid& g, display::window& win, int n, int e, int s, int w )
        : compass(g, win, n, e, s, w, {e, n}, {e, s}, {w, s}, {w, n})
        {};

      grid north() const {
        int near_col = m_northwest.x + 1;
        int near_row = 1;
        int  far_col = m_grid.max_columns() - m_northeast.x;
        int  far_row = m_north;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid northeast() const {
        int near_col = m_grid.max_columns() - (m_northeast.x - 1);
        int near_row = 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_northeast.x;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid east() const {
        int near_col = m_grid.max_columns() - (m_east - 1);
        int near_row = m_northeast.y + 1;
        int  far_col = m_grid.max_columns();
        int  far_row = m_grid.max_rows() - m_southeast.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid southeast() const {
        int near_col = m_grid.max_columns() - (m_southeast.x - 1);
        int near_row = m_grid.max_rows() - (m_southeast.y - 1);
        int  far_col = m_grid.max_columns();;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid south() const {
        int near_col = m_southwest.x + 1;
        int near_row = m_grid.max_rows() - (m_south - 1);
        int  far_col = m_grid.max_columns() - m_southeast.x;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid southwest() const {
        int near_col = 1;
        int near_row = m_grid.max_rows() - (m_southwest.y - 1);
        int  far_col = m_southwest.x;
        int  far_row = m_grid.max_rows();

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid west() const {
        int near_col = 1;
        int near_row = m_northwest.y + 1;
        int  far_col = m_west;
        int  far_row = m_grid.max_rows() - m_southwest.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid northwest() const {
        int near_col = 1;
        int near_row = 1;
        int  far_col = m_northwest.x;
        int  far_row = m_northwest.y;

        return m_grid.subgrid(
            near_col, near_row,
             far_col, far_row
          );
      }

      grid centre() const {
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
          north().bounds().draw(renderer);
        }

        if (m_northeast.x > 0 && m_northeast.y > 0) {
          northeast().bounds().draw(renderer);
        }

        if (m_east > 0) {
          east().bounds().draw(renderer);
        }

        if (m_southeast.x > 0 && m_southeast.y > 0) {
          southeast().bounds().draw(renderer);
        }

        if (m_south > 0) {
          south().bounds().draw(renderer);
        }

        if (m_southwest.x > 0 && m_southwest.y > 0) {
          southwest().bounds().draw(renderer);
        }

        if (m_west > 0) {
          west().bounds().draw(renderer);
        }

        if (m_northwest.x > 0 && m_northwest.y > 0) {
          northwest().bounds().draw(renderer);
        }

        centre().bounds().draw(renderer);
      }
  };

  class northwest_elbo : public isolinear::layout::compass {
    public:
      northwest_elbo(grid& g, display::window& w)
        : isolinear::layout::compass( g, w, 3, 0, 0, 4 )
        {};

      grid sweep() const {
        return northwest();
      }

      grid vertical_control() const {
        return west().left_columns(west().max_columns() - 1);
      }

      grid vertical_gutter() const {
        return west().right_columns(1);
      }

      grid horizontal_control() const {
        return north().top_rows(2);
      }

      grid horizontal_gutter() const {
        return north().bottom_rows(1);
      }

      grid content() const {
        return centre();
      }
  };


}
