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
      LeftRoundCap* lcap = new LeftRoundCap(this->c);
      Region* lcap_region = new Region(
        btn_region->position,
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
        btn_region->position->x
          + btn_region->size->x
          - cap_size->x,
        btn_region->position->y,
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
