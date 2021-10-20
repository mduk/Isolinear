#include "grid.h"

GridRegion Grid::MultiCellRegion(
  int near_col, int near_row,
  int  far_col, int  far_row
) const {

  if (near_col > num_cols || far_col > num_cols) {
    throw std::runtime_error("Out of bounds");
  }

  return GridRegion{ this, near_col, near_row, far_col, far_row };
}


GridRegion Grid::PositionRegion(Position& p) const {
  return SingleCellRegion(
    PositionColumn(p),
    PositionRow(p)
  );
}

GridRegion Grid::SingleCellRegion(
    int col, int row
) const {
  return MultiCellRegion(
    col, row, col, row
  );
}

GridRegion Grid::Row(int row) const {
  return MultiCellRegion(
      1,        row,
      num_cols, row
    );
}

GridRegion Grid::Column(int col) const {
  return MultiCellRegion(
      col, 1,
      col, 12 // Indeterminate
    );
}

Region Grid::CalculateGridRegion(
    GridRegion const& gr
) const {

  int near_col = gr.NearColumn();
  int near_row = gr.NearRow();
  int  far_col = gr.FarColumn();
  int  far_row = gr.FarRow();


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

  return Region{ x, y, w, h };
}

void Grid::DrawCells(SDL_Renderer* renderer) const {
  for (int i=1; i<=num_cols; i++) {
    for (int j=1; j<=num_cols; j++) {
      SingleCellRegion(i, j)
        .Stroke(renderer, 0xfeffffff);
    }
  }
}
