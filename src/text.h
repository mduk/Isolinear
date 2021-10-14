#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>


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

    int Height() {
      return TTF_FontHeight(sdl_font);
    }

    void RenderText(
        SDL_Renderer* renderer,
        Region bounds,
        std::string text
    ) {
      SDL_Surface* surface = TTF_RenderUTF8_Blended(
          sdl_font, text.c_str(), SDL_Color{255,255,255}
      );

      SDL_Texture* texture = SDL_CreateTextureFromSurface(
          renderer, surface
      );

      Size surface_size{surface};
      Region label_region = bounds.AlignSouthEast(surface_size);
      label_region.position.Subtract(Coordinate{5,0});

      SDL_Rect label_rect = label_region.AsSdlRect();
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
