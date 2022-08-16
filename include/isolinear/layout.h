#pragma once

#include <stdio.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <utility>

#include "geometry.h"
#include "control.h"
#include "event.h"
#include "layout.h"
#include "display.h"


namespace isolinear::layout {

    class grid {

    protected:
        geometry::vector m_cell_size{0};
        geometry::vector m_size{0};
        geometry::vector m_gutter{0};
        geometry::region m_bounds{0};
        geometry::vector m_offset{0};

    public:

        grid(
            geometry::region b,
            geometry::vector cs,
            geometry::vector g,
            geometry::vector s,
            geometry::vector o
        ) :
            m_bounds{std::move(b)},
            m_cell_size{cs},
            m_gutter{g},
            m_size{s},
            m_offset{o}
        { };

        grid subgrid(
            int near_col, int near_row,
            int  far_col, int  far_row
        ) const {
          return grid{
              calculate_grid_region(
                  near_col, near_row,
                  far_col, far_row
              ),
              m_cell_size,
              m_gutter,
              geometry::vector(
                  far_col - near_col + 1,
                  far_row - near_row + 1
              ),
              {0}
          };
        }

        grid row(int row) const {
          int cols = max_columns();
          int rows = max_rows();

          row = (row > 0) ? row : rows + row;

          return subgrid(1, row, cols, row);
        }

        grid rows(int from, int to) const {
          int cols = max_columns();
          int rows = max_rows();

          from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

          return subgrid(1, from, cols, to);
        }

        grid column(int col) const {
          int cols = max_columns();
          int rows = max_rows();

          col = (col > 0) ? col : cols + col;

          return subgrid(col, 1, col, rows);
        }

        grid columns(int from, int to) const {
          int rows = max_rows();

          from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

          return subgrid(from, 1, to, rows);
        }

        grid left_columns(int n) const {
          return columns(1, n);
        }

        grid right_columns(int n) const {
          return columns(max_columns() - n + 1, max_columns());
        }

        grid centre_columns(int l, int r) const {
          return columns(l + 1, max_columns() - r);
        }

        grid top_rows(int n) const {
          return rows(1, n);
        }

        grid bottom_rows(int n) const {
          return rows(max_rows() - (n-1), max_rows());
        }

        grid centre_rows(int t, int b) const {
          return rows(t + 1, max_rows() - b);
        }

        geometry::region cell(int col, int row) const {
          return calculate_grid_region(col, row, col, row);
        }

        geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
          geometry::position origin = m_bounds.origin().add(m_offset);
          return geometry::region{
              /* x */ origin.x + (m_cell_size.x * (near_col - 1)),
              /* y */ origin.y + (m_cell_size.y * (near_row - 1)),
              /* w */ m_cell_size.x * ((far_col - near_col) + 1) - m_gutter.x,
              /* h */ m_cell_size.y * ((far_row - near_row) + 1) - m_gutter.y
          };
        }

        void draw(SDL_Renderer* renderer) const {
          for (int i=1; i<=max_columns(); i++) {
            for (int j=1; j<=max_rows(); j++) {
              cell(i, j).fill(renderer, 0x33ffffff);
            }
          }
        }

        int position_column_index(geometry::position p) const {
          return floor((p.x - (m_bounds.X() + m_offset.x)) / m_cell_size.x) + 1;
        }

        int position_row_index(geometry::position p) const {
          return floor((p.y - (m_bounds.Y() + m_offset.y)) / m_cell_size.y) + 1;
        }

        int max_columns() const {
          return m_size.x;
        }

        int max_rows() const {
          return m_size.y;
        }

        geometry::vector gutter() const {
          return m_gutter;
        }

        geometry::region bounds() const {
          return m_bounds;
        }

        void print() const {
          printf("Grid %d,%d (%d,%d)\n",
                 m_bounds.X(), m_bounds.Y(),
                 m_bounds.W(), m_bounds.H()
          );
        }
    };

    class gridfactory {
      protected:
        geometry::vector m_cell_size{100, 50};
        geometry::vector m_size{3, 3};
        geometry::vector m_gutter{0};
        geometry::region m_bounds;
        geometry::vector m_offset{0};
        std::list<grid> m_grids{};

      public:
        gridfactory(
            geometry::region b,
            geometry::vector cs,
            geometry::vector g
        )
            : m_bounds(b), m_cell_size(cs), m_gutter(g), m_size(b.W() / cs.x, b.H() / cs.y),
              m_offset((b.W() % cs.x) / 2, (b.H() % cs.y) / 2) {};

        geometry::vector size() const {
          return m_size;
        }

        grid &root() {
          return subgrid(1, 1, m_size.x, m_size.y);
        }

        grid &subgrid(
            int near_col, int near_row,
            int far_col, int far_row
        ) {
          if (far_col < 0) { far_col = m_size.x + far_col; }
          if (far_row < 0) { far_row = m_size.y + far_row; }

          auto region = calculate_grid_region(
              near_col, near_row,
              far_col, far_row
          );
          geometry::vector size(
              far_col - near_col + 1,
              far_row - near_row + 1
          );
          geometry::vector offset(0, 0);
          m_grids.emplace_back(region, m_cell_size, m_gutter, size, offset);
          return m_grids.back();
        }

        geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
          geometry::position origin = m_bounds.origin().add(m_offset);
          return geometry::region{
              /* x */ origin.x + (m_cell_size.x * (near_col - 1)),
              /* y */ origin.y + (m_cell_size.y * (near_row - 1)),
              /* w */ m_cell_size.x * ((far_col - near_col) + 1) - m_gutter.x,
              /* h */ m_cell_size.y * ((far_row - near_row) + 1) - m_gutter.y
          };
        }

    };

    class compass {
    protected:
        grid &m_grid;

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
            grid &g,
            int n, int e, int s, int w,
            geometry::vector ne, geometry::vector se,
            geometry::vector sw, geometry::vector nw
        ) :
            m_grid(g),
            m_north(n), m_east(e), m_south(s), m_west(w),
            m_northeast(ne), m_southeast(se),
            m_southwest(sw), m_northwest(nw) {}

        compass(grid &g, int n, int e, int s, int w)
            : compass(g, n, e, s, w, {e, n}, {e, s}, {w, s}, {w, n}) {};

        geometry::region bounds() const {
          return m_grid.bounds();
        }

        grid north() const {
          int near_col = m_northwest.x + 1;
          int near_row = 1;
          int far_col = m_grid.max_columns() - m_northeast.x;
          int far_row = m_north;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid northeast() const {
          int near_col = m_grid.max_columns() - (m_northeast.x - 1);
          int near_row = 1;
          int far_col = m_grid.max_columns();
          int far_row = m_northeast.x;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid east() const {
          int near_col = m_grid.max_columns() - (m_east - 1);
          int near_row = m_northeast.y + 1;
          int far_col = m_grid.max_columns();
          int far_row = m_grid.max_rows() - m_southeast.y;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid southeast() const {
          int near_col = m_grid.max_columns() - (m_southeast.x - 1);
          int near_row = m_grid.max_rows() - (m_southeast.y - 1);
          int far_col = m_grid.max_columns();;
          int far_row = m_grid.max_rows();

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid south() const {
          int near_col = m_southwest.x + 1;
          int near_row = m_grid.max_rows() - (m_south - 1);
          int far_col = m_grid.max_columns() - m_southeast.x;
          int far_row = m_grid.max_rows();

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid southwest() const {
          int near_col = 1;
          int near_row = m_grid.max_rows() - (m_southwest.y - 1);
          int far_col = m_southwest.x;
          int far_row = m_grid.max_rows();

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid west() const {
          int near_col = 1;
          int near_row = m_northwest.y + 1;
          int far_col = m_west;
          int far_row = m_grid.max_rows() - m_southwest.y;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid northwest() const {
          int near_col = 1;
          int near_row = 1;
          int far_col = m_northwest.x;
          int far_row = m_northwest.y;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

        grid centre() const {
          int near_col = m_northwest.x + 1;
          int near_row = m_northwest.y + 1;
          int far_col = m_grid.max_columns() - m_southeast.x;
          int far_row = m_grid.max_rows() - m_southeast.y;

          return m_grid.subgrid(
              near_col, near_row,
              far_col, far_row
          );
        }

    };

    class elbo : public isolinear::layout::compass {
      public:
        virtual grid sweep() = 0;
        virtual grid vertical_control() = 0;
        virtual grid vertical_gutter() = 0;
        virtual grid horizontal_control() = 0;
        virtual grid horizontal_gutter() = 0;
        virtual grid content() = 0;

        elbo(grid &g, int h, int v)
          : isolinear::layout::compass(g, h, 0, 0, v) {};

    };

    class northwest_elbo : public elbo {
      public:
        northwest_elbo(grid &g, int h, int v)
          : elbo(g, h, v) {}

      public:
        grid sweep() override {
          return northwest();
        }

        grid vertical_control() override {
          return west().left_columns(west().max_columns() - 1);
        }

        grid vertical_gutter() override {
          return west().right_columns(1);
        }

        grid horizontal_control() override {
          return north().top_rows(2);
        }

        grid horizontal_gutter() override {
          return north().bottom_rows(1);
        }

        grid content() override {
          return centre();
        }
    };

    class vertical_row {
      protected:
        layout::grid m_grid;
        int m_allocated_north = 1;

      public:
        explicit vertical_row(grid g) : m_grid(std::move(g)) {}

        grid allocate_north(int space) {
          auto grid = m_grid.rows(m_allocated_north, m_allocated_north + (space - 1));
          m_allocated_north += space;
          return grid;
        }
    };

    class horizontal_row {
      protected:
        layout::grid m_grid;
        int m_allocated_left = 0;
        int m_allocated_right = 0;

      public:
        explicit horizontal_row(grid g) : m_grid(std::move(g)) {}

        grid allocate_left(int space) {
          auto grid = m_grid.columns(
              m_allocated_left + 1,
              m_allocated_left + space
            );

          m_allocated_left += space;
          return grid;
        }

        grid allocate_right(int space) {
          auto grid = m_grid.columns(
              (m_grid.max_columns() - m_allocated_right) - (space-1),
              m_grid.max_columns() - m_allocated_right
            );
          m_allocated_right += space;
          return grid;
        }

        grid remainder() {
          return m_grid.columns(
              m_allocated_left+1,
              m_grid.max_columns() - (m_allocated_right)
              );
        }
    };

}
