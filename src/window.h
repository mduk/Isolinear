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
    Position2D position;
    Size2D size;
    Grid grid;
    SDL_Renderer* sdl_renderer;

    Window(
        Position2D p,
        Size2D s,
        Vector2D _margin,
        Vector2D _gutter
    ) :
      position{p},
      size{s},
      g_margin{_margin},
      g_gutter{_gutter}
    {
      InitSdl();

      grid = Grid{
          Region2D{size},
          button_font.Height(), // Row height
          g_margin,
          g_gutter
        };
    };

    ~Window() {
      SDL_DestroyRenderer(sdl_renderer);
      SDL_DestroyWindow(sdl_window);
    }

    ColourScheme Colours() {
      return colours;
    }
    void Colours(ColourScheme cs) {
      colours = cs;
      for (auto* drawable : drawables) {
        drawable->Colours(cs);
      }
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
        Region2D clickablearea{drawable->SdlRect()};
        clickablearea.Print();
        if (clickablearea.Encloses(cursor)) {
          drawable->OnMouseButtonDown(event);
          continue;
        }
      }
    }

    void OnWindowResize(SDL_WindowEvent& event) {

      SDL_GetWindowSize(
          sdl_window,
          &size.x,
          &size.y
        );

      grid.ResizeBounds(size);
    }

  protected:
    SDL_Window* sdl_window;

    std::string title{"Isolinear"};
    std::list<Drawable*> drawables;

    ColourScheme colours;

    const Font header_font{ FONT, 96, 0xff0099ff };
    const Font button_font{ FONT, 44, 0xff000000 };

    Vector2D g_margin;
    Vector2D g_gutter;

    void InitSdl() {
      printf("Window::InitSdl()\n");
      printf("        Position: %d,%d\n", position.x, position.y);
      printf("        Size: %d,%d\n", size.x, size.y);
      printf("        Orientation: %s\n", (size.x >= size.y)
                                          ? "landscape"
                                          : "portrait");
      sdl_window = SDL_CreateWindow(
          title.c_str(),
          position.x, position.y,
          size.x, size.y,
          0 | SDL_WINDOW_ALLOW_HIGHDPI
            | SDL_WINDOW_FULLSCREEN_DESKTOP
            //| SDL_WINDOW_BORDERLESS
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
