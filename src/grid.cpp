#include "grid.h"


Size Grid::CellSize() {
  int width = floor(bounds.size.x / num_cols);
  return Size{ width, row_height };
}

Region Grid::SingleCellRegion(
    int col, int row
) {
  return MultiCellRegion(
    col, row, col, row
  );
}

Region Grid::MultiCellRegion(
  int near_col, int near_row,
  int  far_col, int  far_row
) {

  if (near_col > num_cols || far_col > num_cols) {
    throw std::runtime_error("Out of bounds");
  }

  int x, y, w, h;

  Size s = CellSize();

  x = bounds.position.x
    + margin.x
    + (s.x * (near_col - 1))
    ;

  y = bounds.position.y
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

Grid Grid::SubGrid(
    int near_col, int near_row,
    int  far_col, int  far_row
) {
  return Grid{
    MultiCellRegion(
        near_col, near_row,
         far_col,  far_row
      ),
    row_height,
    (far_col - near_col),
    Margin{0,0},
    gutter
  };
}

Region Grid::PositionRegion(Position& p) {
  return SingleCellRegion(
    PositionColumn(p),
    PositionRow(p)
  );
}

int Grid::PositionColumn(Position& p) {
  Size s = CellSize();
  return floor(p.x / s.x) + 1;
}

int Grid::PositionRow(Position& p) {
  return floor(p.y / row_height) + 1;
}


void Grid::DrawCells(SDL_Renderer* renderer) {
  for (int i=1; i<=num_cols; i++) {
    for (int j=1; j<=num_cols; j++) {
      SingleCellRegion(i, j)
        .Stroke(renderer, 0xff0000ff);
    }
  }
}
