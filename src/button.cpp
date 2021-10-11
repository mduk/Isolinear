#include "button.h"


void Button::Draw(SDL_Renderer* renderer) const {
  Colour drawcolour = (this->active == true)
                    ? this->colours.active
                    : this->colours.base;
  boxColor(renderer,
    this->bounds.NearX(), this->bounds.NearY(),
    this->bounds.FarX(), this->bounds.FarY(),
    drawcolour
  );
}


void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button::OnMouseButtonDown\n");
  this->active = !this->active;
}


Region Button::Bounds() {
  return this->bounds;
}
