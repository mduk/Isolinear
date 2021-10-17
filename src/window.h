#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "types.h"
#include "drawable.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"
#include "text.h"

#define FONT "/home/daniel/.fonts/Swiss 911 Ultra Compressed BT.ttf"


using namespace std;


class Window {
  public:
    Size size;
    Grid grid;
    SDL_Renderer* sdl_renderer;

    Window(
        int _w, int _h,
        int _num_columns,
        Coordinate _margin,
        Coordinate _gutter
    ) :
      size{_w, _h},
      g_num_columns{_num_columns},
      g_margin{_margin},
      g_gutter{_gutter}
    {
      InitSdl();

      grid = Grid{
          Region{size},
          header_font.Height(), // Row height
          g_num_columns,
          g_margin,
          g_gutter
        };
    };

    ~Window() {
      for (auto* drawable : drawables) {
        delete drawable;
      }

      SDL_DestroyRenderer(sdl_renderer);
      SDL_DestroyWindow(sdl_window);
    }

    Font const& HeaderFont() const {
      return header_font;
    }

    Font const& ButtonFont() const {
      return button_font;
    }

    void Add(Drawable* drawable) {
      drawables.push_back(drawable);
    }

    void Draw() {
      for (auto* drawable : drawables) {
        drawable->Draw(sdl_renderer);
      }
      SDL_RenderPresent(sdl_renderer);
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      Position cursor{event};

      for (auto* drawable : drawables) {
        if (Region{drawable->SdlRect()}.Encloses(cursor)) {
          drawable->OnMouseButtonDown(event);
        }
      }
    }

    void draw(Position p, uint32_t c) {
      filledCircleColor(sdl_renderer, p.x, p.y, 10, c);
    }

    void draw(Region re, uint32_t c) {
      boxColor(sdl_renderer,
          re.NearX(), re.NearY(),
          re.FarX(),  re.FarY(),
          c
        );
    }

  protected:
    SDL_Window* sdl_window;

    std::string title{"Isolinear"};
    std::list<Drawable*> drawables;

    const Font header_font{ FONT, 96 };
    const Font button_font{ FONT, 64 };

    int g_num_columns;
    Coordinate g_margin;
    Coordinate g_gutter;

    void InitSdl() {
      sdl_window = SDL_CreateWindow(
          title.c_str(),
          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          size.x, size.y,
          0 | SDL_WINDOW_OPENGL
            | SDL_WINDOW_FULLSCREEN_DESKTOP
        );

      if (!sdl_window) {
        throw std::runtime_error(
          "Failed to create SDL window"
        );
      }

      sdl_renderer = SDL_CreateRenderer(
          sdl_window, -1, SDL_RENDERER_SOFTWARE
        );

      if (!sdl_renderer) {
        throw std::runtime_error(
          "Failed to create SDL renderer"
        );
      }

      SDL_SetRenderDrawBlendMode(
          sdl_renderer, SDL_BLENDMODE_BLEND
        );
      SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
      SDL_RenderPresent(sdl_renderer);

      SDL_GetWindowSize(
          sdl_window,
          &size.x,
          &size.y
        );
    }

};
