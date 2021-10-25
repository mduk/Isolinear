#pragma once

#include <list>
#include <exception>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"

class GridRegion;

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
      ) : Grid(b, rh, m, g, 12)
    {};

    Grid(
        Region2D b,
        int rh,
        Vector2D m,
        Vector2D g,
        int nc
      ) :
        bounds{b},
        row_height{rh},
        margin{m},
        gutter{g},
        num_cols{nc}
    {};

    Grid SubGrid(int, int, int, int) const;

    GridRegion MultiCellRegion(int, int, int, int) const;
    GridRegion PositionRegion(Position2D p) const;
    GridRegion SingleCellRegion(int, int) const;
    GridRegion Row(int row) const;
    GridRegion Column(int col) const;

    Region2D CalculateGridRegion(GridRegion const& gr) const;
    Region2D CalculateGridRegion(int, int, int, int) const;
    Region2D CalculateGridRegion(int, int) const;

    void DrawCells(SDL_Renderer* renderer) const;

    Size2D CellSize() const {
      int width = floor(bounds.W() / num_cols);
      return Size2D{ row_height, row_height };
    }

    int PositionColumnIndex(Position2D p) const {
      Size2D s = CellSize();
      return floor(p.x / s.x) + 1;
    }

    int PositionRowIndex(Position2D p) const {
      return floor(p.y / row_height) + 1;
    }

    int MaxColumns() const {
      return num_cols;
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
    int num_cols{12};
    int row_height{100};
    Vector2D gutter{50, 50};
    Vector2D margin{10, 10};
    std::list<GridRegion> regions{};
};

class GridRegion : public Region2D {
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

    virtual Position2D Origin() const override {
      return grid->CalculateGridRegion(*this).Origin();
    }

    virtual Size2D Size() const override {
      return grid->CalculateGridRegion(*this).Size();
    }

    void Print() const {
      Region2D r = grid->CalculateGridRegion(*this);
      printf("GridRegion<%d,%d (%d,%d) %d,%d\n",
          r.Near().x, r.Near().y,
          r.Size().x, r.Size().y,
          r.Far().x, r.Far().y
        );
    }

  protected:
    Grid const* grid;
    int near_col;
    int near_row;
    int far_col;
    int far_row;
};
