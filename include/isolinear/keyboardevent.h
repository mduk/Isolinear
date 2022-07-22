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

    };

}
