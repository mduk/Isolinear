#include "grid.h"


Size Grid::CellSize() const {
  int width = floor(bounds._size.x / num_cols);
  return Size{ width, row_height };
}

GridRegion Grid::SingleCellRegion(
    int col, int row
) const {
  return MultiCellRegion(
    col, row, col, row
  );
}

GridRegion Grid::MultiCellRegion(
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

Grid Grid::SubGrid(
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

GridRegion Grid::PositionRegion(Position& p) const {
  return SingleCellRegion(
    PositionColumn(p),
    PositionRow(p)
  );
}

int Grid::PositionColumn(Position& p) const {
  Size s = CellSize();
  return floor(p.x / s.x) + 1;
}

int Grid::PositionRow(Position& p) const {
  return floor(p.y / row_height) + 1;
}


void Grid::DrawCells(SDL_Renderer* renderer) const {
  for (int i=1; i<=num_cols; i++) {
    for (int j=1; j<=num_cols; j++) {
      SingleCellRegion(i, j)
        .Stroke(renderer, 0xfeffffff);
    }
  }
}
