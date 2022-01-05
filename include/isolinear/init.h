#pragma once

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>


namespace isolinear {

  asio::io_context io_context;
  std::thread io_thread;

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
