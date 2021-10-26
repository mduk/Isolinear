#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "grid.h"
#include "colours.h"
#include "geometry.h"
#include "text.h"

#define FONT "/home/daniel/.fonts/Swiss 911 Ultra Compressed BT.ttf"


using namespace std;


class Window {
  public:
    Size2D size;
    Grid grid;
    SDL_Renderer* sdl_renderer;

    Window(
        int _w, int _h,
        Vector2D _margin,
        Vector2D _gutter
    ) :
      size{_w, _h},
      g_margin{_margin},
      g_gutter{_gutter}
    {
      InitSdl();

      grid = Grid{
          Region2D{size},
          header_font.Height(), // Row height
          g_margin,
          g_gutter
        };
    };

    ~Window() {
      SDL_DestroyRenderer(sdl_renderer);
      SDL_DestroyWindow(sdl_window);
    }

    void SetTitle(std::string newtitle) {
      SDL_SetWindowTitle(sdl_window, newtitle.c_str());
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
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& event) {
      Position2D cursor{event};

      for (auto* drawable : drawables) {
        if (Region2D{drawable->SdlRect()}.Encloses(cursor)) {
          drawable->OnMouseButtonDown(event);
        }
      }
    }

    void OnWindowResize(SDL_WindowEvent& event) {
      printf("Window::OnWindowResize\n");

      printf("        Querying Window Size\n");
      SDL_GetWindowSize(
          sdl_window,
          &size.x,
          &size.y
        );

      printf("        Resizing Window Grid\n");
      grid.ResizeBounds(size);
    }

  protected:
    SDL_Window* sdl_window;

    std::string title{"Isolinear"};
    std::list<Drawable*> drawables;

    const Font header_font{ FONT, 96 };
    const Font button_font{ FONT, 64 };

    int g_num_columns;
    Vector2D g_margin;
    Vector2D g_gutter;

    void InitSdl() {
      sdl_window = SDL_CreateWindow(
          title.c_str(),
          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
          size.x, size.y,
          0 | SDL_WINDOW_OPENGL
            | SDL_WINDOW_RESIZABLE
            //| SDL_WINDOW_FULLSCREEN_DESKTOP
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

      SDL_GetWindowSize(
          sdl_window,
          &size.x,
          &size.y
        );
    }

};
