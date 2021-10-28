#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class Grid {
  public:

    Grid() {}
    Grid(Size2D s) : bounds{Position2D{0,0}, s} {};
    Grid(Region2D b) : bounds{b} {};

    Grid(
        Region2D b,
        int rh,
        Vector2D m,
        Vector2D g
      ) :
        bounds{b},
        row_height{rh},
        margin{m},
        gutter{g}
    {};

    Grid SubGrid(
      int near_col, int near_row,
      int  far_col, int  far_row
    ) const {
      return Grid{
          CalculateGridRegion(
              near_col, near_row,
              far_col, far_row
            ),
          row_height,
          Vector2D{0,0},
          gutter
        };
    }

    Grid Row(int row) const {
      int cols = MaxColumns();
      int rows = MaxRows();

      if (row < 0) {
        row = rows + row;
      }

      return Grid{
          CalculateGridRegion(
              1, row,
              cols, row
            ),
          row_height,
          Vector2D{0,0},
          gutter
        };
    }

    Grid Column(int col) const {
      int cols = MaxColumns();
      int rows = MaxRows();

      if (col < 0) {
        col = cols + col;
      }

      return Grid{
          CalculateGridRegion(
              col, 1,
              col, rows
            ),
          row_height,
          Vector2D{0,0},
          gutter
        };
    }

    Region2D CalculateGridRegion(
      int col, int row
    ) const {
      return CalculateGridRegion(
          col, row, col, row
        );
    }

    Region2D CalculateGridRegion(
      int near_col, int near_row,
      int  far_col, int  far_row
    ) const {
      int x, y, w, h;

      Size2D s = CellSize();

      x = bounds.Origin().x
        + margin.x
        + (s.x * (near_col - 1))
        ;

      y = bounds.Origin().y
        + margin.y
        + row_height * (near_row - 1)
        ;

      w = s.x * ((far_col - near_col) + 1)
        - gutter.x
        ;

      h = row_height * ((far_row - near_row) + 1)
        - gutter.y
        ;

      return Region2D{ x, y, w, h };
    }

    void DrawCells(SDL_Renderer* renderer) const {
      for (int i=1; i<=MaxColumns(); i++) {
        for (int j=1; j<=MaxRows(); j++) {
          CalculateGridRegion(i, j)
            .Fill(renderer, 0x33ffffff);
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
      return floor(p.x / s.x) + 1;
    }

    int PositionRowIndex(Position2D p) const {
      return floor(p.y / row_height) + 1;
    }

    int MaxColumns() const {
      int max_col = PositionColumnIndex(bounds.Far());
      Region2D proposed = CalculateGridRegion(
          max_col, 1
        );

      if (bounds.Encloses(proposed)) {
        return max_col;
      }

      return max_col - 1;
    }

    int MaxRows() const {
      int max_row = PositionRowIndex(bounds.Far());
      Region2D proposed = CalculateGridRegion(
          1, max_row
        );

      if (bounds.Encloses(proposed)) {
        return max_row;
      }

      return max_row - 1;
    }

    void Print() const {
      printf("Grid<0x%08X>%d,%d (%d,%d)\n",
          this,
          bounds.X(), bounds.Y(),
          bounds.W(), bounds.H()
        );
    }

    Region2D bounds;
  protected:
    int row_height{100};
    Vector2D gutter{50, 50};
    Vector2D margin{0,0};
};

