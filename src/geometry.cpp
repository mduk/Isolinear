#include "geometry.h"




void Coordinate::Add(Coordinate* c) {
  this->x += c->x;
  this->y += c->y;
}

void Coordinate::Subtract(Coordinate* c) {
  this->x -= c->x;
  this->y -= c->y;
}





//////////////////////////////////////////////////
// Position
//////////////////////////////////////////////////

void Position::Draw(SDL_Renderer* renderer) {
  this->Draw(renderer, RandomColour());
}

void Position::Draw(SDL_Renderer* renderer, Colour colour) {
  rectangleColor(renderer,
    this->x - 10, this->y - 10,
    this->x + 11, this->y + 11,
    colour
  );
  boxColor(renderer,
    this->x - 10, this->y - 10,
    this->x, this->y,
    colour
  );
  boxColor(renderer,
    this->x, this->y,
    this->x + 10, this->y + 10,
    colour
  );
}






//////////////////////////////////////////////////
// Region
//////////////////////////////////////////////////

bool Region::Encloses(Coordinate& point) {
  if (point.x < this->position.x) return false;
  if (point.y < this->position.y) return false;
  if (point.x > this->position.x + this->size.x) return false;
  if (point.y > this->position.y + this->size.y) return false;
  return true;
}

Region* Region::Draw(SDL_Renderer* renderer) {
  static Colour col = RandomColour();
  return this->Draw(renderer, col);
}

Region* Region::Draw(SDL_Renderer* renderer, Colour colour) {
  boxColor(renderer,
    this->position.x,
    this->position.y,
    this->position.x + this->size.x,
    this->position.y + this->size.y,
    colour
  );
  return this;
}

WindowRegion::WindowRegion(SDL_Window* _window)
    : Region{0,0,0,0}, window{_window}
{
  this->Update();
}

void WindowRegion::Update() {
  int x, y;
  SDL_GetWindowSize(this->window, &x, &y);
  this->size.x = x;
  this->size.y = y;
}
