#pragma once

#include "geometry.h"


namespace isolinear::keyboard {

    using isolinear::geometry::position;


    class event {
    protected:
        SDL_KeyboardEvent m_sdl_keyboardevent;


    public:
        explicit event(SDL_KeyboardEvent e)
          : m_sdl_keyboardevent{e} {};

        bool is_key_up() const {
          return m_sdl_keyboardevent.type == SDL_KEYUP;
        }

        bool is_key_down() const {
          return m_sdl_keyboardevent.type == SDL_KEYDOWN;
        }

    };

}

namespace isolinear::window {
    class event {
    protected:
        SDL_WindowEvent m_sdl_windowevent;

    public:
        event(SDL_WindowEvent e)
            : m_sdl_windowevent{e}
        {}
    };
}