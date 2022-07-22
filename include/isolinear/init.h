#pragma once

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include <asio.hpp>
#include <map>

#include "display.h"
#include "keyboardevent.h"


namespace isolinear {

  asio::io_context io_context;
  std::thread io_thread;
  std::list<display::window> window_list{};
  std::map<uint32_t, display::window&> window_map{};

  void init() {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    io_thread = std::thread([](){ io_context.run(); });
  }

  bool loop() {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
        case SDL_KEYUP:
          if (e.key.keysym.sym == SDLK_ESCAPE) {
            return false;
          }
          window_map.at(e.key.windowID).on_keyboard_event(keyboard::event(e.key));
          break;

        case SDL_MOUSEMOTION:
          window_map.at(e.key.windowID).on_pointer_event(pointer::event(e.motion));
          break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
          window_map.at(e.button.windowID).on_pointer_event(pointer::event(e.button));
          break;

        case SDL_QUIT:
          return false;
      }
    }

    for (auto& window : window_list) {
      window.render();
    }

    return true;
  }

  void shutdown() {
    io_context.stop();
    io_thread.join();
  }

  display::window& new_window(geometry::vector position, geometry::vector size) {
    window_list.emplace_back(position, size);
    auto& window = window_list.back();
    window.colours(isolinear::theme::nightgazer_colours);
    window_map.emplace(window.window_id(), window);
    return window;
  }

  display::window& new_window() {
    auto display = display::detect_displays().back();
    return new_window( {display.x, display.y}, {display.w, display.h} );
  }

}
