#include "table.h"

Table::Table(
    Region* _where,
    int _rowh,
    int _gutter
) :
    where{_where},
    rowh{_rowh},
    gutter{_gutter},
    assigned_regions{}
{ }

Region* Table::CalculateLeftCapRegion(
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

Region* Table::CalculateCellRegion(
    int top_row, int bottom_row,
    int left_col, int right_col
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
    + this->rowh * (top_row - 1);

  w = cell_width * ((right_col - left_col) + 1)
    - (this->gutter * 2);

  h = this->rowh * ((bottom_row - top_row) + 1)
    - (this->gutter * 2);

  return new Region(x, y, w, h);
}

Region* Table::CalculateRightCapRegion(
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

Region* Table::AssignRegion(
    int top_row, int bottom_row,
    int left_col, int right_col,
    Button* button
) {
  Region* region = this->CalculateCellRegion(top_row, bottom_row, left_col, right_col);
  this->AssignRegion(region, button);
}

Region* Table::AssignRegion(
    Region* region,
    Button* button
) {
  TableAssignment* ga = new TableAssignment(region, button);
  this->assigned_regions.push_back(ga);
  return region;
}

void Table::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  Position* mouse_position = new Position(event->x, event->y);
  for (auto const* assignment : this->assigned_regions) {
    if (assignment->region->Encloses(mouse_position)) {
      assignment->button->OnMouseButtonDown(event);
    }
  }
  delete mouse_position;
}

void Table::Draw(
    SDL_Renderer* renderer
) {
  for (auto const* assignment : this->assigned_regions) {
    assignment->button->Draw(renderer, assignment->region);
  }
}

