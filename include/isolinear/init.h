#pragma once

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include <asio.hpp>

#include "display.h"


namespace isolinear {

  asio::io_context io_context;
  std::thread io_thread;
  std::list<display::window> windows{};

  void init() {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    io_thread = std::thread([](){ io_context.run(); });
  }

  void shutdown() {
    io_context.stop();
    io_thread.join();
  }

  display::window& new_window(geometry::vector position, geometry::vector size) {
    windows.emplace_back(position, size);
    return windows.back();
  }

  display::window& new_window() {
    auto display = display::detect_displays().back();
    return new_window( {display.x, display.y}, {display.w, display.h} );
  }


}
