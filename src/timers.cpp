#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <vector>

#include <asio.hpp>
#include <thread>

#include <iostream>
#include <fmt/core.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "miso.h"

#include "colours.h"
#include "compasslayout.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"
#include "progressbar.h"
#include "shapes.h"
#include "sweep.h"
#include "ui.h"
#include "window.h"


using asio::ip::tcp;


bool drawdebug = false;


class timer {
  public:
    asio::io_context& io_context;
    std::chrono::seconds seconds;
    std::chrono::time_point<std::chrono::system_clock> started;
    asio::high_resolution_timer asio_timer;

  public:
    timer(asio::io_context& ioc, std::chrono::seconds s)
      : io_context(ioc)
      , seconds(s)
      , started(std::chrono::system_clock::now())
      , asio_timer(ioc, s)
    {}
};


class timer_row : public isolinear::ui::header_east_bar {
  public:
    timer& m_timer;
  public:
    timer_row(Window& w, Grid g, timer& t)
      : header_east_bar(w, g, fmt::format("{}", t.seconds.count()))
      , m_timer(t)
    {}
};


int main(int argc, char* argv[])
{
  printf("INIT\n");

  srand(time(NULL));

  asio::io_context io_context;
  auto work_guard = asio::make_work_guard(io_context);

  //size_t n_threads = std::thread::hardware_concurrency();
  size_t n_threads = 1;
  std::vector<std::thread> thread_pool;
  for (size_t i = 0; i < n_threads; i++){
    thread_pool.emplace_back([&io_context](){ io_context.run(); });
  }

  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_ShowCursor(SDL_DISABLE);

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

  SDL_Rect display = displays.back();

  Window window(
      Position2D{ display },
      Size2D{ display }
    );

  isolinear::ui::horizontal_button_bar control_bar(window, window.grid.Rows(1,2));
  auto& five_second_button = control_bar.AddButton("5 SEC");
  window.Add(&control_bar);

  std::list<timer> timers;

  miso::connect(five_second_button.signal_press, [&](){
    cout << "Ding!\n";
    five_second_button.Activate();
    timers.emplace_back(io_context, std::chrono::seconds(5));
    auto& timer = timers.back();
    timer.asio_timer.async_wait([&](std::error_code){
      cout << "Dong!\n";
      five_second_button.Deactivate();
    });
  });

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  window.Colours(nightgazer_colours);

  bool running = true;
  SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));

  while (running) {
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawdebug) {
      window.grid.Draw(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case 'g': drawdebug = !drawdebug; break;

            case 'd': window.Colours(debug_colours       ); break;
            case 'r': window.Colours(red_alert_colours   ); break;
            case 'y': window.Colours(yellow_alert_colours); break;
            case 'b': window.Colours(blue_alert_colours  ); break;
            case 'n': window.Colours(nightgazer_colours  ); break;

          }
          break;

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          Position2D pos{x, y};
          int gx = window.grid.PositionColumnIndex(pos),
              gy = window.grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());
          break;
        }

/*
      case SDL_FINGERDOWN:
        printf("TAP\n");
        window.OnPointerEvent(PointerEvent{ e.tfinger, window.size });
        break;
*/

      case SDL_MOUSEBUTTONDOWN:
        printf("CLICK\n");
        window.OnPointerEvent(PointerEvent{ e.button });
        break;

      case SDL_QUIT:
        running = false;
        break;
      }
    }

    window.Update();
    window.Draw();

    // Prune expired timers
    std::list<timer>::iterator it = timers.begin();
    while (it != timers.end()) {
      auto& timer = *it;

      auto t_expires_relative_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(
            timer.asio_timer.expires_from_now()
          ).count();

      if (t_expires_relative_seconds <= 0) {
        timers.erase(it++);
      }
      else {
        it++;
      }
    }

    // Draw Timers
    int i = 1;
    for (auto it = timers.begin(); it != timers.end(); ++it) {
      auto& timer = *it;

      auto t_started_seconds =
        std::chrono::system_clock::to_time_t(timer.started);

      auto t_expires_relative_seconds =
        std::chrono::duration_cast<std::chrono::seconds>(
            timer.asio_timer.expires_from_now()
          ).count();

      int r = (i * 2) + 2;
      timer_row heb(
          window,
          window.grid.Rows(r-1, r),
          timer
        );
      heb.Colours(window.Colours());
      heb.Draw(window.sdl_renderer);
      i++;
    }

    SDL_RenderPresent(window.sdl_renderer);
  }

  io_context.stop();
  work_guard.reset();
  for(auto& thread : thread_pool) {
    thread.join();
  }

  return 0;
}
