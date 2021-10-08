#include "grid.h"


Region Grid::CalculateSingleCellRegion(
    int row, int col
) {
  return this->CalculateMultiCellRegion(
    row, row, col, col
  );
}

Region Grid::CalculateMultiCellRegion(
    int top_row, int bottom_row,
    int left_col, int right_col
) {
  int x, y, w, h;

  int cell_width = floor(bounds.size.x / num_cols);

  x = bounds.position.x
    + (cell_width * (left_col - 1));

  y = bounds.position.y
    + rowh * (top_row - 1);

  w = cell_width * ((right_col - left_col) + 1)
    - (gutter * 2);

  h = rowh * ((bottom_row - top_row) + 1)
    - (gutter * 2);

  return Region{ x, y, w, h };
}

