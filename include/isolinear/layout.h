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

  class compass {
    protected:
      grid& m_grid;

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
          grid& g,
          int n, int e, int s, int w,
          geometry::vector ne, geometry::vector se,
          geometry::vector sw, geometry::vector nw
        ) :
          m_grid(g),
          m_north(n), m_east(e), m_south(s), m_west(w),
          m_northeast(ne), m_southeast(se),
          m_southwest(sw), m_northwest(nw)
      {}

      compass( grid& g, int n, int e, int s, int w )
        : compass(g, n, e, s, w, {e, n}, {e, s}, {w, s}, {w, n})
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

  };

  class northwest_elbo : public isolinear::layout::compass {
    public:
      explicit northwest_elbo(grid& g, int h, int v)
        : isolinear::layout::compass( g, h, 0, 0, v )
        {};

      grid sweep() {
        return northwest();
      }

      grid vertical_control() {
        return west().left_columns(west().max_columns() - 1);
      }

      grid vertical_gutter() {
        return west().right_columns(1);
      }

      grid horizontal_control() {
        return north().top_rows(2);
      }

      grid horizontal_gutter() {
        return north().bottom_rows(1);
      }

      grid content() {
        return centre();
      }
  };

}
