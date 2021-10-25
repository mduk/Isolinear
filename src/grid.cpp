#include "grid.h"

Grid Grid::SubGrid(
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
      gutter,
      (far_col - near_col) + 1
    };
}

Region2D Grid::CalculateGridRegion(
  int col, int row
) const {
  return CalculateGridRegion(
      col, row, col, row
    );
}

Region2D Grid::CalculateGridRegion(
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

void Grid::DrawCells(SDL_Renderer* renderer) const {
  for (int i=1; i<=num_cols; i++) {
    for (int j=1; j<=MaxRows(); j++) {
      CalculateGridRegion(i, j)
        .Fill(renderer, 0x33ffffff);
    }
  }
}
