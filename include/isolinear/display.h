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
#include "keyboardevent.h"
#include "theme.h"
#include "geometry.h"
#include "text.h"

#define FONT "/home/daniel/.fonts/Swiss 911 Ultra Compressed Regular.otf"


namespace isolinear::display {


  class window {

    protected: // Geometry
      geometry::vector m_position;
      geometry::vector m_size;

    protected: // SDL
      SDL_Renderer* m_sdl_renderer{nullptr};
      SDL_Window* m_sdl_window{nullptr};

    protected: // Fonts
      const text::font m_header_font{ FONT, 60, 0xff0099ff };
      const text::font m_button_font{ FONT, 30, 0xff000000 };
      const text::font  m_label_font{ FONT, 30, 0xff0099ff };


    public: // Constructors & Destructors
      window(geometry::vector p, geometry::vector s)
        : m_position{p}, m_size{s}
      {
        init_sdl();
        set_title("Isolinear");
      };

      ~window() {
        SDL_DestroyRenderer(m_sdl_renderer);
        SDL_DestroyWindow(m_sdl_window);
      }

    public: // ui::drawable interface
      theme::colour_scheme colours() {
        return m_colours;
      }

      void colours(theme::colour_scheme cs) {
        m_colours = cs;
        for (auto* drawable : m_drawables) {
          drawable->colours(cs);
        }
      }

      void draw() const {
        for (auto* drawable : m_drawables) {
          drawable->draw(m_sdl_renderer);
        }
      }

      void render() const {
        theme::colour unpack_colour = (m_override_background > 0)
                                    ? m_override_background
                                    : m_colours.background;

        uint8_t blue = unpack_colour;
        uint8_t green = unpack_colour >> 8;
        uint8_t red = unpack_colour >> 16;
        uint8_t alpha = unpack_colour >> 24;

        SDL_SetRenderDrawColor(m_sdl_renderer, red, green, blue, alpha);
        SDL_RenderClear(m_sdl_renderer);

        draw();

        SDL_RenderPresent(m_sdl_renderer);
      }

      void on_pointer_event(pointer::event event) {
        set_title(fmt::format("Mouse X={} Y={}", event.Position().x, event.Position().y));

        for (auto* drawable : m_drawables) {
          drawable->on_pointer_event(event);
        }
      }

      void on_keyboard_event(keyboard::event event) {
        m_override_background = 0xffffffff;
      }

    public: // Accessors
      [[nodiscard]] text::font const& header_font() const { return m_header_font; }
      [[nodiscard]] text::font const& button_font() const { return m_button_font; }
      [[nodiscard]] text::font const& label_font()  const { return m_label_font; }
      [[nodiscard]] geometry::vector size() const { return m_size; }
      [[nodiscard]] geometry::region region() const { return geometry::region{m_position, m_size}; }
      [[nodiscard]] SDL_Renderer* renderer() const { return m_sdl_renderer; }

    protected: // Protected window properties
      std::string m_title{"Isolinear"};
      std::list<ui::drawable*> m_drawables;
      theme::colour_scheme m_colours;
      theme::colour m_override_background = 0x00000000;

  public: // Public window methods
      void set_title(const std::string& new_title) {
        SDL_SetWindowTitle(m_sdl_window, new_title.c_str());
      }

      void add(ui::drawable* drawable) {
        m_drawables.push_back(drawable);
      }

      Uint32 window_id() const {
        return SDL_GetWindowID(m_sdl_window);
      }

    protected: // Protected window methods
      void init_sdl() {
        m_sdl_window = SDL_CreateWindow(
            m_title.c_str(),
            m_position.x, m_position.y,
            m_size.x, m_size.y,
            0 | SDL_WINDOW_ALLOW_HIGHDPI
              | SDL_WINDOW_RESIZABLE
            //  | SDL_WINDOW_FULLSCREEN_DESKTOP // Take up the screen that is focused
              //| SDL_WINDOW_BORDERLESS
          );

        if (!m_sdl_window) {
          throw std::runtime_error(
            "Failed to create SDL window"
          );
        }

        m_sdl_renderer = SDL_CreateRenderer(
            m_sdl_window, -1, SDL_RENDERER_SOFTWARE
          );

        if (!m_sdl_renderer) {
          throw std::runtime_error(
            "Failed to create SDL renderer"
          );
        }

        SDL_SetRenderDrawBlendMode(
            m_sdl_renderer, SDL_BLENDMODE_BLEND
          );

        SDL_GetWindowSize(
            m_sdl_window,
            &m_size.x,
            &m_size.y
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
