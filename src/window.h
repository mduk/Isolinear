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

    void Add(Drawable*);
    void Draw();
    void OnMouseButtonDown(SDL_MouseButtonEvent&);

    SDL_Renderer* sdl_renderer;
    void draw(Position, uint32_t c);
    void draw(Region, uint32_t c);

  protected:
    void InitSdl();

    SDL_Window* sdl_window;

    std::string title{"Isolinear"};
    std::list<Drawable*> drawables;

    const Font header_font{ FONT, 96 };
    const Font button_font{ FONT, 64 };

    int g_num_columns;
    Coordinate g_margin;
    Coordinate g_gutter;


};
