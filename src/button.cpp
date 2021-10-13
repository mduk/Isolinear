#include "button.h"


Button::Button(Region b, ColourScheme cs, Label l)
  : bounds{b}, colours{cs}, label{l} {
}

void Button::Draw(SDL_Renderer* renderer) const {
  Colour drawcolour = (this->active == true)
                    ? this->colours.active
                    : this->colours.base;
  boxColor(renderer,
    this->bounds.NearX(), this->bounds.NearY(),
    this->bounds.FarX(), this->bounds.FarY(),
    drawcolour
  );

  bounds.RenderText(renderer, label);
}


void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button::OnMouseButtonDown\n");
  active = !active;
  if (active) {
    printf("      Now Active\n");
  }
  else {
    printf("      Now Inactive\n");
  }
}


Region Button::Bounds() {
  return this->bounds;
}
