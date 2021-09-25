#include "grid.h"

Grid::Grid(
    Region* _where,
    int _rowh,
    int _gutter
) :
    where{_where},
    rowh{_rowh},
    gutter{_gutter}
{
  this->rows.push_back(new TitleRow(this));
  this->rows.push_back(new ContainerRow(this));
  this->rows.push_back(new ImageRow(this));
  this->rows.push_back(new VolumeRow(this));
  this->rows.push_back(new Volume2Row(this));
  this->rows.push_back(new Volume3Row(this));
}

void Grid::Draw(
    SDL_Renderer* renderer
) {
  int row_n = 1;
  for (auto const& row : this->rows) {
    Region* row_region = new Region(
      this->where->position->x,
      this->where->position->y + (this->rowh * (row_n - 1)),
      this->where->size->x,
      this->rowh
    );
    row->Draw(renderer, row_region);
    delete row_region;
    row_n++;
  }
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

void Grid::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  printf("Grid click!\n");
}

void GridRow::Draw(SDL_Renderer* renderer, Region* where) {
  int x = where->position->x;
  int y = where->position->y;
  int g = 8;

  for (auto const& c : this->cells) {
    Region* cell_region = new Region(
      x + g, y + g,
      c->width - g, where->size->y - g
    );
    c->Draw(renderer, cell_region);
    delete cell_region;

    x += c->width;
  }
}

void GridCell::Draw(SDL_Renderer* renderer, Region* where) {
  this->button->Draw(renderer, where);
}

ContainerRow::ContainerRow(Grid* _g) : GridRow{_g} {
  Button* cap = new Button();
  cap->lcap = true;
  cap->c = RandomColour();
  this->cells.push_back(new GridCell(cap, this->grid->rowh));

  Button* button = new Button();
  button->c = cap->c;
  button->lcap = true;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 1));

  Button* rrcap = new Button();
  rrcap->rcap = true;
  rrcap->c = cap->c;
  this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
}

ImageRow::ImageRow(Grid* _g) : GridRow{_g} {
  Button* cap = new Button();
  cap->lcap = true;
  cap->c = RandomColour();
  this->cells.push_back(new GridCell(cap, this->grid->rowh));

  Button* button = new Button();
  button->c = cap->c;
  button->lcap = true;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));

  Button* rrcap = new Button();
  rrcap->rcap = true;
  rrcap->c = RandomColour();
  this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
}

VolumeRow::VolumeRow(Grid* _g) : GridRow{_g} {
  Button* lrcap = new Button();
  lrcap->lcap = true;
  lrcap->c = RandomColour();
  this->cells.push_back(new GridCell(lrcap, this->grid->rowh));

  Button* button = new Button();
  button->c = lrcap->c;
  button->lcap = true;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 3));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 3));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 3));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 3));

  Button* rrcap = new Button();
  rrcap->rcap = true;
  rrcap->c = RandomColour();
  this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
}

Volume2Row::Volume2Row(Grid* _g) : GridRow{_g} {
  Button* lrcap = new Button();
  lrcap->lcap = true;
  lrcap->c = RandomColour();
  this->cells.push_back(new GridCell(lrcap, this->grid->rowh));

  Button* button = new Button();
  button->c = lrcap->c;
  button->lcap = true;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 4));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 4));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 4));

  Button* rrcap = new Button();
  rrcap->rcap = true;
  rrcap->c = RandomColour();
  this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
}

Volume3Row::Volume3Row(Grid* _g) : GridRow{_g} {
  Button* lrcap = new Button();
  lrcap->lcap = true;
  lrcap->c = RandomColour();
  this->cells.push_back(new GridCell(lrcap, this->grid->rowh));

  Button* button = new Button();
  button->c = lrcap->c;
  button->lcap = true;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 6));
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 6));

  Button* rrcap = new Button();
  rrcap->rcap = true;
  rrcap->c = RandomColour();
  this->cells.push_back(new GridCell(rrcap, this->grid->rowh));
}
