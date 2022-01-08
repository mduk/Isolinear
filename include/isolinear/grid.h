#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


namespace isolinear {


  using isolinear::geometry::vector;
  using isolinear::geometry::Region2D;
  using isolinear::geometry::Size2D;
  using isolinear::geometry::Position2D;


  class grid {

    protected:
      int row_height{100};
      vector size{3,3};

    public:
      vector gutter{50, 50};
      Region2D bounds;

      grid() {};

      grid(
          Region2D b,
          int rh,
          vector g,
          vector s
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
            CalculateGridRegion(
                near_col, near_row,
                far_col, far_row
              ),
            row_height,
            gutter,
            vector(
                far_col - near_col + 1,
                far_row - near_row + 1
              )
          };
      }

      grid Row(int row) const {
        int cols = MaxColumns();
        int rows = MaxRows();

        row = (row > 0) ? row : rows + row;

        return SubGrid(1, row, cols, row);
      }

      grid Rows(int from, int to) const {
        int cols = MaxColumns();
        int rows = MaxRows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return SubGrid(1, from, cols, to);
      }

      grid Column(int col) const {
        int cols = MaxColumns();
        int rows = MaxRows();

        col = (col > 0) ? col : cols + col;

        return SubGrid(col, 1, col, rows);
      }

      grid Columns(int from, int to) const {
        int cols = MaxColumns();
        int rows = MaxRows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return SubGrid(from, 1, to, rows);
      }

      grid LeftColumns(int n) const {
        return Columns(1, n);
      }

      grid RightColumns(int n) const {
        return Columns(MaxColumns() - n + 1, MaxColumns());
      }

      grid CentreColumns(int l, int r) const {
        return Columns(l + 1, MaxColumns() - r);
      }

      grid TopRows(int n) const {
        return Rows(1, n);
      }

      grid BottomRows(int n) const {
        return Rows(MaxRows() - n, MaxRows());
      }

      grid MiddleRows(int t, int b) const {
        return Rows(t + 1, MaxRows() - b);
      }

      Region2D Cell(int col, int row) const {
        return CalculateGridRegion(col, row, col, row);
      }

      Region2D CalculateGridRegion(int near_col, int near_row, int far_col, int far_row) const {
        Position2D origin = bounds.Origin();
        Size2D cell_size = CellSize();
        return Region2D{
          /* x */ origin.x + (cell_size.x * (near_col - 1)),
          /* y */ origin.y + (cell_size.y * (near_row - 1)),
          /* w */ cell_size.x * ((far_col - near_col) + 1) - gutter.x,
          /* h */ cell_size.y * ((far_row - near_row) + 1) - gutter.y
        };
      }

      void Draw(SDL_Renderer* renderer) const {
        for (int i=1; i<=MaxColumns(); i++) {
          for (int j=1; j<=MaxRows(); j++) {
            Cell(i, j).Fill(renderer, 0x33ffffff);
          }
        }
      }

      void ResizeBounds(Size2D size) {
        bounds.Resize(size);
      }

      Size2D CellSize() const {
        return Size2D{ row_height*2, row_height };
      }

      int PositionColumnIndex(Position2D p) const {
        Size2D s = CellSize();
        return floor((p.x - bounds.X()) / s.x) + 1;
      }

      int PositionRowIndex(Position2D p) const {
        return floor((p.y - bounds.Y()) / row_height) + 1;
      }

      int MaxColumns() const {
        return size.x;
      }

      int MaxRows() const {
        return size.y;
      }

      vector Gutter() const {
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
