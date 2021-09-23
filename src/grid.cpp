#include "grid.h"

Grid::Grid(
    int _nrows,
    int _rowh,
    int _roww,
    int _gutter
) : nrows{_nrows},
    rowh{_rowh},
    roww{_roww},
    gutter{_gutter} {
  this->rows.push_back(new ContainerRow(this));
  this->rows.push_back(new ImageRow(this));
}

void Grid::Draw(
    SDL_Renderer* renderer,
    Position* gridpos
) {
  int row_n = 1;
  for (auto const& row : this->rows) {
    Region* row_region = new Region(
      gridpos->x,
      gridpos->y + (this->rowh * (row_n - 1)),
      this->roww, this->rowh
    );
    row->Draw(renderer, row_region);
    delete row_region;
    row_n++;
  }
}

void GridRow::Draw(SDL_Renderer* renderer, Region* where) {
  int x = where->position->x;
  int y = where->position->y;
  int g = 10;

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

  Button* label = new Button();
  label->c = 0xff000000;
  this->cells.push_back(new GridCell(label, this->grid->rowh * 1.618 * 3));

  Button* button = new Button();
  button->c = cap->c;
  button->lcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));
}

ImageRow::ImageRow(Grid* _g) : GridRow{_g} {
  Button* cap = new Button();
  cap->lcap = true;
  cap->c = RandomColour();
  this->cells.push_back(new GridCell(cap, this->grid->rowh));

  Button* button = new Button();
  button->c = cap->c;
  button->rcap = true;
  this->cells.push_back(new GridCell(button, this->grid->rowh * 1.618 * 2));

  Button* button2 = new Button();
  button2->c = cap->c;
  button2->lcap = true;
  button2->rcap = true;
  this->cells.push_back(
      new GridCell(button2, this->grid->rowh * 1.618 * 2)
  );
}
