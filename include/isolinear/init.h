#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

namespace isolinear {

  asio::io_context io_context;
  std::vector<std::thread> io_threads;

  void init() {
    srand(time(NULL));

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    size_t n_threads = std::thread::hardware_concurrency();
    for (size_t i = 0; i < n_threads; i++){
      io_threads.emplace_back([](){ io_context.run(); });
    }
  }

  void shutdown() {
    io_context.stop();

    for(auto& thread : io_threads) {
      thread.join();
    }
  }

  std::vector<SDL_Rect> detect_displays() {
    int number_of_displays = SDL_GetNumVideoDisplays();
    std::vector<SDL_Rect> displays;

    for (int i = 0; i < number_of_displays; i++) {
      SDL_Rect bounds{};
      SDL_GetDisplayBounds(i, &bounds);
      displays.push_back(bounds);

      printf("%d: %d,%d +(%d,%d) [%d]\n",
          i,
          bounds.w,
          bounds.h,
          bounds.x,
          bounds.y,
          bounds.w * bounds.h);
    }

    return displays;
  }

}
