#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


using namespace std;


class Font {
  public:
    Font(std::string p, int s, Colour c)
        : path{p}, size_pt{s}, colour{c}
    {
      sdl_font = TTF_OpenFont(path.c_str(), size_pt);

      if (!sdl_font) {
        fprintf(stderr, "Couldn't load font: %s\n", TTF_GetError());
        throw std::runtime_error(
          "Failed to load font"
        );
      }
    };

    ~Font() {
      TTF_CloseFont(sdl_font);
    }

    int Height() const {
      return TTF_FontHeight(sdl_font);
    }

    void RenderText(
        SDL_Renderer* renderer,
        Region2D bounds,
        Compass align,
        std::string text
    ) const {
      uint8_t r = colour,
              g = colour >> 8,
              b = colour >> 16;

      SDL_Surface* surface = TTF_RenderUTF8_Blended(
          sdl_font, text.c_str(), SDL_Color{r,g,b}
      );

      SDL_Texture* texture = SDL_CreateTextureFromSurface(
          renderer, surface
      );
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

      Region2D label_region = bounds.Align(align, Size2D{surface});
      label_region.Origin().Subtract(Vector2D{5,0});

      SDL_Rect label_rect = label_region.SdlRect();
      SDL_RenderCopy(renderer, texture, NULL, &label_rect);

      SDL_FreeSurface(surface);
      SDL_DestroyTexture(texture);
    };

  protected:
    std::string path;
    int size_pt;
    Colour colour;
    TTF_Font* sdl_font;
};

class FontScheme{
  public:
    Font header;

    FontScheme(Font h)
        : header{h}
      {};
};
