#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"


namespace isolinear::text {

  using isolinear::geometry::region;
  using isolinear::compass;


  class rendered_text {
    protected:
      TTF_Font* m_sdl_font;
      theme::colour m_colour;
      std::string m_text;

      SDL_Surface* m_sdl_surface;

    public:
      rendered_text(TTF_Font* f, theme::colour c, std::string t)
        : m_sdl_font{f}, m_colour{c}, m_text{t}
      {
        uint8_t r = m_colour,
                g = m_colour >> 8,
                b = m_colour >> 16;
        SDL_Color colour{r,g,b};
        m_sdl_surface = TTF_RenderUTF8_Blended(m_sdl_font, m_text.c_str(), colour);
      };

      ~rendered_text() {
        SDL_FreeSurface(m_sdl_surface);
      }

      geometry::vector size() const {
        return geometry::vector{m_sdl_surface};
      }

      void draw(SDL_Renderer* renderer, compass alignment, region bounds) const {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, m_sdl_surface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        region label_region = bounds.align(alignment, size());
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

  protected:
      std::string path;
      int size_pt;
      theme::colour colour;
      TTF_Font* sdl_font;

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

      int height() const {
        return TTF_FontHeight(sdl_font);
      }

      void render_text(
          SDL_Renderer* renderer,
          region bounds,
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

        region label_region = bounds.align(align, geometry::vector{surface->w, surface->h});
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

      rendered_text render_text(theme::colour colour, std::string text) const {
        return rendered_text{sdl_font, colour, text};
      }
  };


}
