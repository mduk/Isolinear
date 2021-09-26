#include "grid.h"

Grid::Grid(
    Region* _where,
    int _rowh,
    int _gutter
) :
    where{_where},
    rowh{_rowh},
    gutter{_gutter}
{ }

Region* Grid::CalculateLeftCapRegion(
    int row
) {
  int x, y, w, h;

  x = this->where->position->x
    + this->gutter;

  y = this->where->position->y
    + this->gutter
    + this->rowh * (row - 1);

  w = this->rowh - (this->gutter * 2);
  h = this->rowh - (this->gutter * 2);

  return new Region(x, y, w, h);
}

Region* Grid::CalculateCellRegion(
    int row,
    int left_col,
    int right_col
) {
  int x, y, w, h;

  int inner_row_width = this->where->size->x - (this->rowh * 2);
  int cell_width = floor(inner_row_width / this->num_cols);

  x = this->where->position->x
    + this->gutter
    + this->rowh
    + (cell_width * (left_col - 1));

  y = this->where->position->y
    + this->gutter
    + this->rowh * (row - 1);

  w = cell_width * ((right_col - left_col) + 1)
    - (this->gutter * 2);

  h = this->rowh - (this->gutter * 2);

  return new Region(x, y, w, h);
}

Region* Grid::CalculateRightCapRegion(
    int row
) {
  int x, y, w, h;

  int inner_row_width = this->where->size->x - (this->rowh * 2);
  int cell_width = floor(inner_row_width / this->num_cols);

  x = this->where->position->x
    + this->gutter
    + this->rowh
    + (cell_width * (this->num_cols + 0));

  y = this->where->position->y
    + this->gutter
    + this->rowh * (row - 1);

  w = this->rowh - (this->gutter * 2);
  h = this->rowh - (this->gutter * 2);

  return new Region(x, y, w, h);
}

void Grid::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  printf("Grid click!\n");
}

void Grid::Draw(
    SDL_Renderer* renderer
) {
}

