#include "grid.h"


Region Grid::SingleCellRegion(
    int row, int col
) {
  return this->MultiCellRegion(
    row, row, col, col
  );
}

Region Grid::MultiCellRegion(
    int top_row, int bottom_row,
    int left_col, int right_col
) {
  int x, y, w, h;

  int cell_width = floor(bounds.size.x / num_cols);

  x = bounds.position.x
    + gutter
    + (cell_width * (left_col - 1));

  y = bounds.position.y
    + gutter
    + rowh * (top_row - 1);

  w = cell_width * ((right_col - left_col) + 1)
    - (gutter * 1);

  h = rowh * ((bottom_row - top_row) + 1)
    - (gutter * 1);

  return Region{ x, y, w, h };
}

Region Grid::PositionRegion(Position& p) {
  return this->SingleCellRegion(
    this->PositionRow(p),
    this->PositionColumn(p)
  );
}

Column Grid::PositionColumn(Position& p) {
  int cell_width = floor(bounds.size.x / num_cols);
  return floor(p.x / cell_width)+1;
}

Row Grid::PositionRow(Position& p) {
  return floor(p.y / rowh)+1;
}
