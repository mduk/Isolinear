#include "grid.h"


Size Grid::CellSize() {
  int width = floor(bounds.size.x / num_cols);
  return Size{ width, rowh };
}

Region Grid::SingleCellRegion(
    Column col, Row row
) {
  return this->MultiCellRegion(
    row, row, col, col
  );
}

Region Grid::MultiCellRegion(
    Row top_row,     Row bottom_row,
    Column left_col, Column right_col
) {
  int x, y, w, h;

  Size s = this->CellSize();

  x = bounds.position.x
    + gutter
    + (s.x * (left_col - 1));

  y = bounds.position.y
    + gutter
    + rowh * (top_row - 1);

  w = s.x * ((right_col - left_col) + 1)
    - (gutter * 1);

  h = rowh * ((bottom_row - top_row) + 1)
    - (gutter * 1);

  return Region{ x, y, w, h };
}

Region Grid::PositionRegion(Position& p) {
  return this->SingleCellRegion(
    this->PositionColumn(p),
    this->PositionRow(p)
  );
}

Column Grid::PositionColumn(Position& p) {
  Size s = this->CellSize();
  return floor(p.x / s.x)+1;
}

Row Grid::PositionRow(Position& p) {
  return floor(p.y / rowh)+1;
}
