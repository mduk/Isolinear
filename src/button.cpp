#include "button.h"

using namespace std;

void LeftRoundCap::Draw(
      SDL_Renderer* renderer,
      Region* where
) {
  filledCircleColor(renderer,
    where->position->x + where->size->x,
    where->position->y + where->size->y / 2,
    where->size->x,
    this->bg_colour
  );
}

void RightBarCap::Draw(
      SDL_Renderer* renderer,
      Region* where
) {
    int g = 5;
    boxColor(renderer,
      where->position->x + g, where->position->y,
      where->position->x + where->size->x, where->position->y + where->size->y,
      0xFF00FF00
    );
}

void LeftIndicatorCap::Draw(
      SDL_Renderer* renderer,
      Region* where
) {
    int g = 5;

    boxColor(renderer,
      where->position->x, where->position->y,

      where->position->x + where->size->x - g,
      where->position->y + where->size->x - g,

      0xFF00FF00
    );

    boxColor(renderer,
      where->position->x, where->position->y + where->size->x + g,
      where->position->x + where->size->x - g,
      where->position->y + (where->size->x * 2),
      0xFF0000FF
    );

}

void RightRoundCap::Draw(
      SDL_Renderer* renderer,
      Region* where
) {
  filledCircleColor(renderer,
    where->position->x,
    where->position->y + (where->size->y / 2),
    where->size->x,
    this->bg_colour
  );
}

void Button::Draw(SDL_Renderer* renderer) {
  Region* rect_region = this->region->Copy();
  this->region->Draw(renderer, 0xff000000);

  if (this->lcap || this->rcap) {
    Size* cap_size = new Size(
      this->region->size->y / 2,
      this->region->size->y
    );
    if (this->lcap) {
      LeftRoundCap* lcap = new LeftRoundCap(this->c);
      Region* lcap_region = new Region(
        this->region->position,
        cap_size
      );
      lcap->Draw(renderer, lcap_region);
      delete lcap, lcap_region;

      rect_region->position->x = rect_region->position->x
        + cap_size->x;
      rect_region->size->x = rect_region->size->x
        - cap_size->x;
    }

    if (this->rcap) {
      RightRoundCap* rcap = new RightRoundCap(this->c);
      Region* rcap_region = new Region(
        this->region->position->x
          + this->region->size->x
          - cap_size->x,
        this->region->position->y,
        cap_size->x,
        cap_size->y
      );

      rcap->Draw(renderer, rcap_region);
      delete rcap, rcap_region;

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

void Button::OnMouseButtonDown(SDL_MouseButtonEvent* event) {
  switch (event->button) {
    case SDL_BUTTON_LEFT: {
      Colour c = RandomColour();
      while (c == this->c) {
        c = RandomColour();
      }
      this->c = c;

      this->lcap = (rand() % 3) == 1;
      this->rcap = (rand() % 3) == 1;
      break;
    }

    case SDL_BUTTON_RIGHT: {
      this->lcap = false;
      this->rcap = false;
      this->c = 0xff000000;
      break;
    }
  }
}
