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
        window(SDL_WindowEvent e)
            : m_sdl_windowevent{e}
        {}
    };


    enum pointer_type { MOUSE, FINGER };


    class pointer {

    protected:
        geometry::position m_position;
        pointer_type m_type;

    public:
        explicit pointer(SDL_MouseMotionEvent e) : m_position{e.x, e.y}, m_type{MOUSE} {};
        explicit pointer(SDL_MouseButtonEvent e) : m_position{e.x, e.y}, m_type{MOUSE}  {};

        pointer(SDL_TouchFingerEvent e, geometry::vector ws)
            : m_position{
                static_cast<int>(ws.x * e.x),
                static_cast<int>(ws.y * e.y)
              },
              m_type{FINGER} {};

        geometry::position Position() {
          return m_position;
        }
    };

}
