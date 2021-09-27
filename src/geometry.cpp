#include "geometry.h"

Coordinate::Coordinate(unsigned int _x, unsigned int _y)
  : x{_x},
    y{_y} { }

Coordinate* Coordinate::Copy() {
  Coordinate* copy = new Coordinate(
    this->x, this->y
  );
  return copy;
}

void Coordinate::Add(Coordinate* c) {
  this->x += c->x;
  this->y += c->y;
}

void Coordinate::Subtract(Coordinate* c) {
  this->x -= c->x;
  this->y -= c->y;
}

bool Coordinate::IsWithin(Region* r) {
  return r->Encloses(this);
}

Position::Position(unsigned int _x, unsigned int _y)
  : Coordinate{_x, _y} { }

Size::Size(unsigned int _w, unsigned int _h)
  : Coordinate{_w, _h} { }

Region::Region(Position* _p, Size* _s)
  : position{_p},
    size{_s} { }

Region::Region(
    unsigned int _x,
    unsigned int _y,
    unsigned int _w,
    unsigned int _h
) {
  this->position = new Position(_x, _y);
  this->size = new Size(_w, _h);
}

bool Region::Encloses(Coordinate* point) {
  if (point->x < this->position->x) return false;
  if (point->y < this->position->y) return false;
  if (point->x > this->position->x + this->size->x) return false;
  if (point->y > this->position->y + this->size->y) return false;
  return true;
}

Region* Region::Copy() {
  Position* position = new Position(
    this->position->x,
    this->position->y
  );
  Size* size = new Size(
    this->size->x,
    this->size->y
  );
  Region* copy = new Region(position, size);
  return copy;
}

Region* Region::Draw(SDL_Renderer* renderer) {
  return this->Draw(renderer, RandomColour());
}

Region* Region::Draw(SDL_Renderer* renderer, Colour colour) {
  boxColor(renderer,
    this->position->x,
    this->position->y,
    this->position->x + this->size->x,
    this->position->y + this->size->y,
    colour
  );
  return this;
}
