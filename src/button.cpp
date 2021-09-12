#include "button.h"

using namespace std;

void LeftRoundCap::Draw(
      SDL_Renderer* renderer,
      Position* at,
      Size* size
) {
  filledCircleColor(renderer,
    at->x + size->x, at->y + size->y / 2,
    size->x,
    this->bg_colour
  );
}

void RightBarCap::Draw(
      SDL_Renderer* renderer,
      Position* at,
      Size* size
) {
    int g = 5;
    boxColor(renderer,
      at->x + g, at->y,
      at->x + size->x, at->y + size->y,
      0xFF00FF00
    );
}

void LeftIndicatorCap::Draw(
      SDL_Renderer* renderer,
      Position* at,
      Size* size
) {
    int g = 5;

    boxColor(renderer,
      at->x, at->y,

      at->x + size->x - g,
      at->y + size->x - g,

      0xFF00FF00
    );

    boxColor(renderer,
      at->x, at->y + size->x + g,
      at->x + size->x - g,
      at->y + (size->x * 2),
      0xFF0000FF
    );

}

void RightRoundCap::Draw(
      SDL_Renderer* renderer,
      Position* at,
      Size* size
) {
  filledCircleColor(renderer,
    at->x, at->y + (size->y / 2),
    size->x,
    this->bg_colour
  );
}

Button::Button() {
  this->c = 0xFF0099FF;
}

void Button::Draw(SDL_Renderer* renderer, Region* btn_region) {
  Region* rect_region = btn_region->Copy();
  printf("drawing button: %s\n\tx,y = %d,%d\n\tw,h = %d,%d\n",
      this->label.c_str(),
      btn_region->position->x,
      btn_region->position->y,
      btn_region->size->x,
      btn_region->size->y
    );

  if (this->lcap || this->rcap) {
    Size* cap_size = new Size(
      btn_region->size->y / 2,
      btn_region->size->y
    );
    if (this->lcap) {
      LeftRoundCap* cap = new LeftRoundCap(this->c);
      cap->Draw(renderer, btn_region->position, cap_size);
      delete cap;

      rect_region->position->x = rect_region->position->x
        + cap_size->x;
      rect_region->size->x = rect_region->size->x
        - cap_size->x;
    }

    if (this->rcap) {
      RightRoundCap* cap = new RightRoundCap(this->c);
      Position rcap_at = {
        btn_region->position->x
          + btn_region->size->x
          - cap_size->x,
        btn_region->position->y
      };
      cap->Draw(renderer, &rcap_at, cap_size);
      delete cap;

      rect_region->size->x = rect_region->size->x
        - cap_size->x;
    }

    delete cap_size;
  }

  boxColor(renderer,
    rect_region->position->x,
    rect_region->position->y,
    rect_region->position->x + rect_region->size->x,
    rect_region->position->y + rect_region->size->y,
    this->c
  );
}
