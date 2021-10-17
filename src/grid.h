#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "geometry.h"

class Grid;

class GridRegion : public Region {
  public:
    GridRegion(int x, int y, int w, int h)
      : Region{x, y, w, h}
    {};
};

class Grid {
  public:

    Grid()
    {}

    Grid(Size s)
      : bounds{Position{0,0}, s}
    {};

    Grid(Region b)
      : bounds{b}
    {};

    Grid(
        Region b,
        int rh,
        int nc,
        Margin m,
        Gutter g
      ) :
        bounds{b},
        row_height{rh},
        num_cols{nc},
        margin{m},
        gutter{g}
    {};

    Size CellSize() const {
      int width = floor(bounds.W() / num_cols);
      return Size{ width, row_height };
    }

    Grid SubGrid(
        int near_col, int near_row,
        int  far_col, int  far_row
    ) const {
      return Grid{
        MultiCellRegion(
            near_col, near_row,
             far_col,  far_row
          ),
        row_height,
        (far_col - near_col) + 1,
        Margin{0,0},
        gutter
      };
    }

    int PositionColumn(Position& p) const {
      Size s = CellSize();
      return floor(p.x / s.x) + 1;
    }

    int PositionRow(Position& p) const {
      return floor(p.y / row_height) + 1;
    }

    GridRegion PositionRegion(Position& p) const {
      return SingleCellRegion(
        PositionColumn(p),
        PositionRow(p)
      );
    }

    GridRegion SingleCellRegion(
        int col, int row
    ) const {
      return MultiCellRegion(
        col, row, col, row
      );
    }

    GridRegion MultiCellRegion(
      int near_col, int near_row,
      int  far_col, int  far_row
    ) const {

      if (near_col > num_cols || far_col > num_cols) {
        throw std::runtime_error("Out of bounds");
      }

      int x, y, w, h;

      Size s = CellSize();

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

      return GridRegion{ x, y, w, h };
    }

    GridRegion Row(int row) const {
      return MultiCellRegion(
          1,        row,
          num_cols, row
        );
    }

    GridRegion Column(int col) const {
      return MultiCellRegion(
          col, 1,
          col, 12 // Indeterminate
        );
    }

    void DrawCells(SDL_Renderer* renderer) const {
      for (int i=1; i<=num_cols; i++) {
        for (int j=1; j<=num_cols; j++) {
          SingleCellRegion(i, j)
            .Stroke(renderer, 0xfeffffff);
        }
      }
    }

    Region bounds;
  protected:
    int num_cols{12};
    int row_height{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
    std::list<GridRegion> regions{};
};

