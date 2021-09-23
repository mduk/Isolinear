#include "grid.h"

Colour RandomColour() {
  std::vector <Colour> colours {
    0xff3f403f, 0xff99ffff, 0xff66ccff, 0xff3399ff,
    0xff664466, 0xffcc99cc, 0xffffcc99, 0xffcc6633,
    0xff996600, 0xffcc9999, 0xffff9999, 0xffcc99cc,
    0xff9966cc, 0xff6666cc, 0xff6699ff, 0xff0099ff,
    0xff66ccff, 0xffcc8866, 0xffbb5544, 0xffaa7799,
    0xff664477, 0xff4466dd, 0xff3355aa, 0xff2266bb,
    0xff5599ee, 0xffffddcc, 0xffff9955, 0xffff6633,
    0xffee1100, 0xff880000, 0xff55aabb, 0xff1144bb,
    0xff112288
  };

  return colours[rand() % colours.size()];
}

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
}
