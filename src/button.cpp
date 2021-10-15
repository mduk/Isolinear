#include "button.h"


void Button::Draw(SDL_Renderer* renderer) const {
  Colour drawcolour = active == true
                    ? colours.active
                    : colours.base;
  boxColor(renderer,
      NearX(), NearY(),
      FarX(),  FarY(),
      drawcolour
    );

  window.ButtonFont().RenderTextSouthEast(
      renderer,
      Region{position, size},
      label
    );
}


void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button::OnMouseButtonDown\n");
  Region::OnMouseButtonDown(event);
  active = !active;
}
