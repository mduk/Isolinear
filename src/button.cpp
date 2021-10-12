#include "button.h"


Button::Button(Region b, ColourScheme cs, Label l)
  : bounds{b}, colours{cs}, label{l} {
  this->sdl_font = TTF_OpenFont(
      "/home/daniel/.fonts/Swiss 911 Ultra Compressed BT.ttf",
      64
    );

  if (!this->sdl_font) {
    fprintf(stderr, "Couldn't load font: %s\n", TTF_GetError());
    throw std::runtime_error(
      "Failed to load font"
    );
  }
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

  SDL_Surface* surface =
    TTF_RenderUTF8_Blended(
        this->sdl_font,
        label.c_str(),
        SDL_Color{255,255,255}
      );

  SDL_Texture* texture =
    SDL_CreateTextureFromSurface(
        renderer, surface
      );

  Size label_size{ surface->w, surface->h };
  Region label_region = bounds.SouthEast(label_size);
  label_region.position.Subtract(Coordinate{5,0});

  SDL_Rect label_rect = label_region.AsSdlRect();
  SDL_RenderFillRect(renderer, &label_rect);

  SDL_RenderCopy(
      renderer,
      texture,
      NULL,
      &label_rect
    );

  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}


void Button::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Button::OnMouseButtonDown\n");
  this->active = !this->active;
}


Region Button::Bounds() {
  return this->bounds;
}
