#pragma once

#include <exception>
#include <list>
#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "pointerevent.h"
#include "drawable.h"
#include "pointerevent.h"
#include "theme.h"
#include "geometry.h"
#include "text.h"

#define FONT "/home/daniel/.fonts/Swiss 911 Ultra Compressed Regular.otf"


namespace isolinear::display {


  class window {

    protected: // Geometry
      geometry::vector _position;
      geometry::vector _size;

    protected: // SDL
      SDL_Renderer* _sdl_renderer;
      SDL_Window* _sdl_window;

    protected: // Fonts
      const text::font _header_font{ FONT, 96, 0xff0099ff };
      const text::font _button_font{ FONT, 44, 0xff000000 };
      const text::font  _label_font{ FONT, 44, 0xff0099ff };


    public: // Constructors & Destructors
      window(geometry::vector p, geometry::vector s)
        : _position{p}, _size{s}
      {
        init_sdl();
      };

      ~window() {
        SDL_DestroyRenderer(_sdl_renderer);
        SDL_DestroyWindow(_sdl_window);
      }

    public: // ui::drawable interface
      theme::colour_scheme Colours() {
        return colours;
      }

      void Colours(theme::colour_scheme cs) {
        colours = cs;
        for (auto* drawable : drawables) {
          drawable->Colours(cs);
        }
      }

      void Draw() {
        SDL_SetRenderDrawColor(_sdl_renderer, 0, 0, 0, 255);
        SDL_RenderClear(_sdl_renderer);

        for (auto* drawable : drawables) {
          drawable->Draw(_sdl_renderer);
        }
      }

      void OnPointerEvent(pointer::event event) {
        for (auto* drawable : drawables) {
          geometry::region bounds = drawable->Bounds();
          if (bounds.encloses(event.Position())) {
            drawable->OnPointerEvent(event);
            continue;
          }
        }
      }

    public: // Accessors
      text::font const& HeaderFont() const { return _header_font; }
      text::font const& ButtonFont() const { return _button_font; }
      text::font const& LabelFont()  const { return _label_font; }

      geometry::vector const size() const { return _size; }

      SDL_Renderer* renderer() const { return _sdl_renderer; }

    protected: // Protected window properties
      std::string title{"Isolinear"};
      std::list<ui::drawable*> drawables;
      theme::colour_scheme colours;

    public: // Public window methods
      void Title(std::string newtitle) {
        SDL_SetWindowTitle(_sdl_window, newtitle.c_str());
      }

      void add(ui::drawable* drawable) {
        drawables.push_back(drawable);
      }

    protected: // Protected window methods
      void init_sdl() {
        _sdl_window = SDL_CreateWindow(
            title.c_str(),
            _position.x, _position.y,
            _size.x, _size.y,
            0 | SDL_WINDOW_ALLOW_HIGHDPI
              //| SDL_WINDOW_FULLSCREEN_DESKTOP // Take up the screen that is focused
              | SDL_WINDOW_BORDERLESS
          );

        if (!_sdl_window) {
          throw std::runtime_error(
            "Failed to create SDL window"
          );
        }

        _sdl_renderer = SDL_CreateRenderer(
            _sdl_window, -1, SDL_RENDERER_SOFTWARE
          );

        if (!_sdl_renderer) {
          throw std::runtime_error(
            "Failed to create SDL renderer"
          );
        }

        SDL_SetRenderDrawBlendMode(
            _sdl_renderer, SDL_BLENDMODE_BLEND
          );

        SDL_GetWindowSize(
            _sdl_window,
            &_size.x,
            &_size.y
          );
      }

  };


  std::vector<SDL_Rect> detect_displays() {
    std::vector<SDL_Rect> displays;

    fmt::print("Detecting displays:\n");

    int number_of_displays = SDL_GetNumVideoDisplays();
    for (int i = 0; i < number_of_displays; i++) {
      SDL_Rect bounds{};
      SDL_GetDisplayBounds(i, &bounds);
      displays.push_back(bounds);

      fmt::print("  {}: {},{} +({},{}) [{}]\n",
          i,
          bounds.w, bounds.h,
          bounds.x, bounds.y,
          bounds.w * bounds.h
        );
    }

    return displays;
  }


}
