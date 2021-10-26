#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


using namespace std;


class Font {
  public:
    Font(std::string p, int s)
        : path{p}, size_pt{s}
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
      SDL_Surface* surface = TTF_RenderUTF8_Blended(
          sdl_font, text.c_str(), SDL_Color{255,255,255}
      );

      SDL_Texture* texture = SDL_CreateTextureFromSurface(
          renderer, surface
      );
      SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

      Size2D surface_size{surface};

      Region2D label_region;
      switch (align) {
        case    Compass::CENTRE: label_region = bounds.AlignCentre(surface_size); break;
        case     Compass::NORTH: label_region = bounds.AlignNorth(surface_size); break;
        case Compass::NORTHEAST: label_region = bounds.AlignNorthEast(surface_size); break;
        case      Compass::EAST: label_region = bounds.AlignEast(surface_size); break;
        case Compass::SOUTHEAST: label_region = bounds.AlignSouthEast(surface_size); break;
        case     Compass::SOUTH: label_region = bounds.AlignSouth(surface_size); break;
        case Compass::SOUTHWEST: label_region = bounds.AlignSouthWest(surface_size); break;
        case      Compass::WEST: label_region = bounds.AlignWest(surface_size); break;
        case Compass::NORTHWEST: label_region = bounds.AlignNorthWest(surface_size); break;
      }

      label_region.Origin().Subtract(Vector2D{5,0});

      SDL_Rect label_rect = label_region.SdlRect();
      SDL_RenderCopy(renderer, texture, NULL, &label_rect);

      SDL_FreeSurface(surface);
      SDL_DestroyTexture(texture);
    };

  protected:
    std::string path;
    int size_pt;
    TTF_Font* sdl_font;
};

class FontScheme{
  public:
    Font header;

    FontScheme(Font h)
        : header{h}
      {};
};
