#include "grid.h"

Grid::Grid(
    Region* _where,
    int _rowh,
    int _gutter
) :
    where{_where},
    rowh{_rowh},
    gutter{_gutter},
    assigned_regions{}
{ }

Region* Grid::CalculateCellRegion(
    int top_row, int bottom_row,
    int left_col, int right_col
) {
  int x, y, w, h;

  int cell_width = floor(this->where->size->x / this->num_cols);

  x = this->where->position->x
    + (cell_width * (left_col - 1));

  y = this->where->position->y
    + this->rowh * (top_row - 1);

  w = cell_width * ((right_col - left_col) + 1)
    - (this->gutter * 2);

  h = this->rowh * ((bottom_row - top_row) + 1)
    - (this->gutter * 2);

  return new Region(x, y, w, h);
}

Region* Grid::AssignCellRegion(
    int top_row, int bottom_row,
    int left_col, int right_col,
    Drawable* drawable
) {
  Region* region = this->CalculateCellRegion(
      top_row, bottom_row,
      left_col, right_col
  );
  return this->AssignRegion(region, drawable);
}

Region* Grid::AssignRegion(
    Region* region,
    Drawable* drawable
) {
  GridAssignment* ga = new GridAssignment(region, drawable);
  this->assigned_regions.push_back(ga);
  return region;
}

void Grid::OnMouseButtonDown(
    SDL_MouseButtonEvent* event
) {
  Position* mouse_position = new Position(event->x, event->y);
  for (auto const* assignment : this->assigned_regions) {
    if (assignment->region->Encloses(mouse_position)) {
      assignment->drawable->OnMouseButtonDown(event);
    }
  }
  delete mouse_position;
}

void Grid::Draw(
    SDL_Renderer* renderer
) {
  for (auto const* assignment : this->assigned_regions) {
    assignment->drawable->Draw(renderer, assignment->region);
  }
}

