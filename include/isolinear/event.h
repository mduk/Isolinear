#pragma once

#include "geometry.h"


namespace isolinear::event {

    class keyboard {
    protected:
        SDL_KeyboardEvent m_sdl_keyboardevent;


    public:
        explicit keyboard(SDL_KeyboardEvent e)
          : m_sdl_keyboardevent{e} {};

        bool is_key_up() const {
          return m_sdl_keyboardevent.type == SDL_KEYUP;
        }

        bool is_key_down() const {
          return m_sdl_keyboardevent.type == SDL_KEYDOWN;
        }

        bool is_repeat() const {
          return m_sdl_keyboardevent.repeat;
        }

    };


    class window {

    protected:
        SDL_WindowEvent m_sdl_windowevent;

    public:
        explicit window(SDL_WindowEvent e)
            : m_sdl_windowevent{e}
        {}
    };


    enum pointer_type { MOUSE, FINGER };


    class pointer {

    protected:
        geometry::position m_position;
        pointer_type m_type;
        bool m_is_mouse_down{false};
        bool m_is_mouse_up{false};

    public:
        explicit pointer(SDL_MouseMotionEvent e) : m_position{e.x, e.y}, m_type{MOUSE} {};
        explicit pointer(SDL_MouseButtonEvent e)
          : m_position{e.x, e.y}
          , m_type{MOUSE}
          , m_is_mouse_down{e.type == SDL_MOUSEBUTTONDOWN}
          , m_is_mouse_up{e.type == SDL_MOUSEBUTTONUP}
          {};

        pointer(SDL_TouchFingerEvent e, geometry::vector ws)
            : m_position{
                static_cast<int>(ws.x * e.x),
                static_cast<int>(ws.y * e.y)
              },
              m_type{FINGER} {};

        geometry::position position() const {
          return m_position;
        }

        event::pointer_type type() const {
          return m_type;
        }

        bool is_mouse_down() const {
          return m_is_mouse_down;
        }

        bool is_mouse_up() const {
          return m_is_mouse_up;
        }
    };

}
