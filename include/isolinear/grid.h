#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


namespace isolinear {


  class grid {

    protected:
      int row_height{100};
      geometry::vector size{3,3};

    public:
      geometry::vector gutter{50, 50};
      geometry::region bounds;

      grid() {};

      grid(
          geometry::region b,
          int rh,
          geometry::vector g,
          geometry::vector s
        ) :
          bounds{b},
          row_height{rh},
          gutter{g},
          size{s}
      {
        if (row_height % 2 == 1) {
          row_height++;
        }
      };

      grid SubGrid(
        int near_col, int near_row,
        int  far_col, int  far_row
      ) const {
        return grid{
            calculate_grid_region(
                near_col, near_row,
                far_col, far_row
              ),
            row_height,
            gutter,
            geometry::vector(
                far_col - near_col + 1,
                far_row - near_row + 1
              )
          };
      }

      grid Row(int row) const {
        int cols = max_columns();
        int rows = max_rows();

        row = (row > 0) ? row : rows + row;

        return SubGrid(1, row, cols, row);
      }

      grid Rows(int from, int to) const {
        int cols = max_columns();
        int rows = max_rows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return SubGrid(1, from, cols, to);
      }

      grid Column(int col) const {
        int cols = max_columns();
        int rows = max_rows();

        col = (col > 0) ? col : cols + col;

        return SubGrid(col, 1, col, rows);
      }

      grid Columns(int from, int to) const {
        int cols = max_columns();
        int rows = max_rows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return SubGrid(from, 1, to, rows);
      }

      grid LeftColumns(int n) const {
        return Columns(1, n);
      }

      grid RightColumns(int n) const {
        return Columns(max_columns() - n + 1, max_columns());
      }

      grid CentreColumns(int l, int r) const {
        return Columns(l + 1, max_columns() - r);
      }

      grid TopRows(int n) const {
        return Rows(1, n);
      }

      grid BottomRows(int n) const {
        return Rows(max_rows() - n, max_rows());
      }

      grid MiddleRows(int t, int b) const {
        return Rows(t + 1, max_rows() - b);
      }

      geometry::region cell(int col, int row) const {
        return calculate_grid_region(col, row, col, row);
      }

      geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
        geometry::position origin = bounds.origin();
        auto cs = cell_size();
        return geometry::region{
          /* x */ origin.x + (cs.x * (near_col - 1)),
          /* y */ origin.y + (cs.y * (near_row - 1)),
          /* w */ cs.x * ((far_col - near_col) + 1) - gutter.x,
          /* h */ cs.y * ((far_row - near_row) + 1) - gutter.y
        };
      }

      void Draw(SDL_Renderer* renderer) const {
        for (int i=1; i<=max_columns(); i++) {
          for (int j=1; j<=max_rows(); j++) {
            cell(i, j).fill(renderer, 0x33ffffff);
          }
        }
      }

      geometry::vector cell_size() const {
        return geometry::vector{ row_height*2, row_height };
      }

      int position_column_index(geometry::position p) const {
        auto s = cell_size();
        return floor((p.x - bounds.X()) / s.x) + 1;
      }

      int position_row_index(geometry::position p) const {
        return floor((p.y - bounds.Y()) / row_height) + 1;
      }

      int max_columns() const {
        return size.x;
      }

      int max_rows() const {
        return size.y;
      }

      geometry::vector Gutter() const {
        return gutter;
      }

      void Print() const {
        printf("Grid %d,%d (%d,%d)\n",
            bounds.X(), bounds.Y(),
            bounds.W(), bounds.H()
          );
      }
  };


}
