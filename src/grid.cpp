#include "grid.h"


Size Grid::CellSize() {
  int width = floor(bounds.size.x / num_cols);
  return Size{ width, rowh };
}

Region Grid::SingleCellRegion(
    Column col, Row row
) {
  return this->MultiCellRegion(
    col, row, col, row
  );
}

Region Grid::MultiCellRegion(
  Column near_col, Row near_row,
  Column  far_col, Row  far_row
) {

  if (near_col > num_cols || far_col > num_cols) {
    throw std::runtime_error("Out of bounds");
  }

  int x, y, w, h;

  Size s = this->CellSize();

  x = bounds.position.x
    + margin.x
    + (s.x * (near_col - 1))
    ;

  y = bounds.position.y
    + margin.y
    + rowh * (near_row - 1)
    ;

  w = s.x * ((far_col - near_col) + 1)
    - gutter.x
    ;

  h = rowh * ((far_row - near_row) + 1)
    - gutter.y
    ;

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
  return floor(p.x / s.x) + 1;
}

Row Grid::PositionRow(Position& p) {
  return floor(p.y / rowh) + 1;
}
