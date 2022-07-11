#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


namespace isolinear {


  class grid {

    protected:
      int m_row_height{100};
      geometry::vector m_size{3,3};
      geometry::vector m_gutter{50, 50};

    public:
      geometry::region m_bounds;

      grid() {};

      grid(
          geometry::region b,
          int rh,
          geometry::vector g,
          geometry::vector s
        ) :
          m_bounds{b},
          m_row_height{rh},
          m_gutter{g},
          m_size{s}
      {
        if (m_row_height % 2 == 1) {
          m_row_height++;
        }
      };

      grid subgrid(
        int near_col, int near_row,
        int  far_col, int  far_row
      ) const {
        return grid{
            calculate_grid_region(
                near_col, near_row,
                far_col, far_row
              ),
            m_row_height,
            m_gutter,
            geometry::vector(
                far_col - near_col + 1,
                far_row - near_row + 1
              )
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
        int cols = max_columns();
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
        return rows(max_rows() - n, max_rows());
      }

      grid centre_rows(int t, int b) const {
        return rows(t + 1, max_rows() - b);
      }

      geometry::region cell(int col, int row) const {
        return calculate_grid_region(col, row, col, row);
      }

      geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
        auto mc = max_columns();
        auto mr = max_rows();

        if (near_col > mc || far_col > mc || near_row > mr || far_row > mr) {
          throw std::out_of_range(fmt::format(
              "Grid region {},{} x {},{} falls outside of the grid bounds, {} x {}",
              near_col, near_row, far_col, far_row, mc, mr));
        }

        geometry::position origin = m_bounds.origin();
        auto cs = cell_size();
        return geometry::region{
          /* x */ origin.x + (cs.x * (near_col - 1)),
          /* y */ origin.y + (cs.y * (near_row - 1)),
          /* w */ cs.x * ((far_col - near_col) + 1) - m_gutter.x,
          /* h */ cs.y * ((far_row - near_row) + 1) - m_gutter.y
        };
      }

      void draw(SDL_Renderer* renderer) const {
        for (int i=1; i<=max_columns(); i++) {
          for (int j=1; j<=max_rows(); j++) {
            cell(i, j).fill(renderer, 0x33ffffff);
          }
        }
      }

      geometry::vector cell_size() const {
        return geometry::vector{ m_row_height*2, m_row_height };
      }

      int position_column_index(geometry::position p) const {
        auto s = cell_size();
        return floor((p.x - m_bounds.X()) / s.x) + 1;
      }

      int position_row_index(geometry::position p) const {
        return floor((p.y - m_bounds.Y()) / m_row_height) + 1;
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


}
