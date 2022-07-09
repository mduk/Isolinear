#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


namespace isolinear::text {

  using isolinear::geometry::Region2D;
  using isolinear::compass;


  class rendered_text {
    protected:
      TTF_Font* sdl_font;
      theme::colour colour;
      std::string text;

      SDL_Surface* sdl_surface;

    public:
      rendered_text(TTF_Font* f, theme::colour c, std::string t)
        : sdl_font{f}, colour{c}, text{t}
      {
        uint8_t r = colour,
                g = colour >> 8,
                b = colour >> 16;
        SDL_Color colour{r,g,b};
        sdl_surface = TTF_RenderUTF8_Blended(sdl_font, text.c_str(), colour);
      };

      ~rendered_text() {
        SDL_FreeSurface(sdl_surface);
      }

      geometry::vector size() const {
        return geometry::vector{sdl_surface};
      }

      void Draw(SDL_Renderer* renderer, compass alignment, Region2D bounds) const {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, sdl_surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        Region2D label_region = bounds.align(alignment, size());
        SDL_Rect label_rect{
            label_region.X(),
            label_region.Y(),
            label_region.W(),
            label_region.H()
          };

        SDL_RenderCopy(renderer, texture, NULL, &label_rect);
        SDL_DestroyTexture(texture);
      }
  };

  class font {
    public:
      font(std::string p, int s, theme::colour c)
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

      ~font() {
        TTF_CloseFont(sdl_font);
      }

      int Height() const {
        return TTF_FontHeight(sdl_font);
      }

      void RenderText(
          SDL_Renderer* renderer,
          Region2D bounds,
          compass align,
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

        Region2D label_region = bounds.align(align, geometry::vector{surface->w, surface->h});
        SDL_Rect label_rect{
            label_region.X(),
            label_region.Y(),
            label_region.W(),
            label_region.H()
          };
        SDL_RenderCopy(renderer, texture, NULL, &label_rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
      };

      rendered_text RenderText(theme::colour colour, std::string text) const {
        return rendered_text{sdl_font, colour, text};
      }

    protected:
      std::string path;
      int size_pt;
      theme::colour colour;
      TTF_Font* sdl_font;
  };


}
