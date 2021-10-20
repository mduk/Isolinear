#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "geometry.h"

class GridRegion;

class Grid {
  public:

    Grid() {}
    Grid(Size s) : bounds{Position{0,0}, s} {};
    Grid(Region b) : bounds{b} {};

    Grid(
        Region b,
        int rh,
        Margin m,
        Gutter g
      ) :
        bounds{b},
        row_height{rh},
        margin{m},
        gutter{g}
    {};

    GridRegion MultiCellRegion(int, int, int, int) const;
    GridRegion PositionRegion(Position p) const;
    GridRegion SingleCellRegion(int, int) const;
    GridRegion Row(int row) const;
    GridRegion Column(int col) const;

    Region CalculateGridRegion(GridRegion const& gr) const;
    Region CalculateGridRegion(int, int, int, int) const;
    Region CalculateGridRegion(int, int) const;

    void DrawCells(SDL_Renderer* renderer) const;

    Size CellSize() const {
      int width = floor(bounds.W() / num_cols);
      return Size{ width, row_height };
    }

    int PositionColumnIndex(Position p) const {
      Size s = CellSize();
      return floor(p.x / s.x) + 1;
    }

    int PositionRowIndex(Position p) const {
      return floor(p.y / row_height) + 1;
    }

    int MaxColumns() const {
      return num_cols;
    }

    int MaxRows() const {
      int max_row = PositionRowIndex(bounds.Far());
      Region proposed = CalculateGridRegion(
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

    Region bounds;
  protected:
    int num_cols{12};
    int row_height{100};
    Gutter gutter{50, 50};
    Margin margin{10, 10};
    std::list<GridRegion> regions{};
};

class GridRegion : public Region {
  public:
    GridRegion(
        Grid const* g,
        int nc, int nr,
        int fc, int fr
      ) :
        grid{g},
        near_col{nc},
        near_row{nr},
        far_col{fc},
        far_row{fr}
    {};

    int NearColumn() const { return near_col; }
    int NearRow()    const { return near_row; }
    int FarColumn()  const { return far_col;  }
    int FarRow()     const { return far_row;  }

    Position Origin() const {
      return grid->CalculateGridRegion(*this).Origin();
    }

    Size GetSize() const {
      return grid->CalculateGridRegion(*this).GetSize();
    }

    void Print() const {
      Region r = grid->CalculateGridRegion(*this);
      printf("%d,%d %d,%d\n",
          r.Origin().x, r.Origin().y,
          r.GetSize().x, r.GetSize().y
        );
    }

  protected:
    Grid const* grid;
    int near_col;
    int near_row;
    int far_col;
    int far_row;
};
