#include <sstream>
#include <vector>

#include <asio.hpp>
#include <thread>

#include <iostream>
#include <fmt/core.h>
#include <fmt/chrono.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "miso.h"

#include "init.h"
#include "theme.h"
#include "drawable.h"
#include "geometry.h"
#include "grid.h"
#include "pointerevent.h"
#include "ui.h"
#include "display.h"
#include "window.h"


using std::cout;


bool drawdebug = false;


namespace isolinear {


  class timer {
    public:
      asio::io_context& io_context;
      std::chrono::seconds seconds;
      std::chrono::time_point<std::chrono::system_clock> started;
      asio::high_resolution_timer asio_timer;

    public:
      miso::signal<> signal_expired;

    public:
      timer(asio::io_context& ioc, long int s)
        : timer(ioc, std::chrono::seconds(s))
      {}

      timer(asio::io_context& ioc, std::chrono::seconds s)
        : io_context(ioc)
        , seconds(s)
        , started(std::chrono::system_clock::now())
        , asio_timer(ioc, s)
      {
        asio_timer.async_wait([&](std::error_code){
          emit signal_expired();
        });
      }

    public:
      int expires_in_seconds() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
            asio_timer.expires_from_now()
          ).count();
      }
  };


  class timer_row : public isolinear::ui::header_east_bar {
    protected:
      isolinear::ui::button& add_time;

    public:
      timer& m_timer;

    public:
      timer_row(display::window& w, isolinear::grid g, timer& t)
        : header_east_bar(w, g, "")
        , m_timer(t)
        , add_time(AddButton("ADD 10 SEC"))
      {
        m_timer.asio_timer.async_wait([&](std::error_code){
          cout << "Clang!\n";
        });

        miso::connect(add_time.signal_press, [&](){
          cout << "add time?\n";
        });

        Update();
      }

      void Update() {
        Label(fmt::format(
            "{:%H:%M:%S} {}/{}",
            m_timer.started,
            m_timer.expires_in_seconds(),
            m_timer.seconds
          ));
      }
  };




  template <class T>
  class button_bar_list : public isolinear::ui::drawable_list<T> {

    public:
      button_bar_list(isolinear::grid g)
        : isolinear::ui::drawable_list<T>(g)
      {}

    protected:
      isolinear::grid grid_for_index(int index) override {
        int far_row = index * 2;
        return isolinear::ui::drawable_list<T>::grid.Rows(far_row-1, far_row);
      }
  };


}


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;


  isolinear::init();

  auto work_guard = asio::make_work_guard(isolinear::io_context);
  auto display = isolinear::display::detect_displays().back();

  geometry::vector display_position{ display.x, display.y };
  geometry::vector display_size{ display.w, display.h };

  isolinear::display::window window(display_position, display_size);

  isolinear::grid grid(
      geometry::Region2D(0, 0, display_size.x, display_size.y),
      window.ButtonFont().Height(), // Row height
      geometry::vector(10,10),
      geometry::vector(25,28)
    );

  isolinear::ui::horizontal_button_bar control_bar(window, grid.Rows(1,2));
  auto& five_second_button = control_bar.AddButton("5 SEC");
  window.Add(&control_bar);

  std::list<isolinear::timer> timers;
  isolinear::button_bar_list<isolinear::timer_row> timer_rows(grid.Rows(3, grid.MaxRows()));
  window.Add(&timer_rows);

  miso::connect(five_second_button.signal_press, [&](){
    cout << "Ding!\n";

    five_second_button.Activate();

    timers.emplace_back(isolinear::io_context, 5);
    auto& timer = timers.back();

    int r = (timer_rows.size() * 2) + 2;
    timer_rows.emplace_back(
        window,
        timer_rows.next_grid(),
        timer
      );

    miso::connect(timer.signal_expired, [&](){
      five_second_button.Deactivate();
    });
  });

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  window.Colours(isolinear::theme::nightgazer_colours);

  bool running = true;
  SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));

  while (running) {
    if (drawdebug) {
      grid.Draw(window.renderer());
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

            case 'd': window.Colours(isolinear::theme::debug_colours       ); break;
            case 'r': window.Colours(isolinear::theme::red_alert_colours   ); break;
            case 'y': window.Colours(isolinear::theme::yellow_alert_colours); break;
            case 'b': window.Colours(isolinear::theme::blue_alert_colours  ); break;
            case 'n': window.Colours(isolinear::theme::nightgazer_colours  ); break;

          }
          break;

        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          geometry::Position2D pos{x, y};
          int gx = grid.PositionColumnIndex(pos),
              gy = grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());
          break;
        }

/*
      case SDL_FINGERDOWN:
        printf("TAP\n");
        window.OnPointerEvent(pointer::event{ e.tfinger, window.size() });
        break;
*/

      case SDL_MOUSEBUTTONDOWN:
        printf("CLICK\n");
        window.OnPointerEvent(pointer::event{ e.button });
        break;

      case SDL_QUIT:
        running = false;
        break;
      }
    }

    window.Update();
    window.Draw();

    // Prune expired timers
    if (false) {
      std::list<isolinear::timer>::iterator it = timers.begin();
      while (it != timers.end()) {
        auto& timer = *it;

        auto t_expires_relative_seconds = timer.expires_in_seconds();

        if (t_expires_relative_seconds <= 0) {
          timers.erase(it++);
        }
        else {
          it++;
        }
      }
    }


    // Draw Timers
    if (false) for (auto it = timer_rows.begin(); it != timer_rows.end(); ++it) {
      auto& timer_row = *it;

      timer_row.Colours(window.Colours());
      timer_row.Update();
      timer_row.Draw(window.renderer());
    }

    SDL_RenderPresent(window.renderer());
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

