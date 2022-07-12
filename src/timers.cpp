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
#include "timer.h"


using std::cout;


bool drawdebug = false;


namespace isolinear {

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
        Colours(w.Colours());

        miso::connect(add_time.signal_press, [&](){
          m_timer.add_ticks(10);
        });
      }

      virtual std::string label() const override {
        return fmt::format(
            "{:%H:%M:%S} {}/{}",
            m_timer.started,
            m_timer.ticks_elapsed,
            m_timer.ticks_remaining
          );
      }

      virtual theme::colour LeftCapColour() const override {
        return (m_timer.ticks_remaining == 0)
          ? Colours().disabled
          : Colours().light;
      }

      virtual theme::colour RightCapColour() const override {
        return LeftCapColour();
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
      geometry::region(0, 0, display_size.x, display_size.y),
      window.ButtonFont().Height(), // Row height
      geometry::vector(10,10),
      geometry::vector(25,28)
    );

  isolinear::ui::horizontal_button_bar control_bar(window, grid.rows(1,2));
  auto& five_second_button = control_bar.AddButton("5 SEC");
  window.add(&control_bar);

  std::list<isolinear::timer> timers;
  isolinear::ui::layout_vertical<isolinear::timer_row, 2> timer_rows(grid.rows(3, grid.max_rows()));
  window.add(&timer_rows);

  miso::connect(five_second_button.signal_press, [&](){
    cout << "Click!\n";

    five_second_button.activate();

    timers.emplace_back(isolinear::io_context, 5);
    auto& timer = timers.back();

    miso::connect(timer.signal_tick, [&](unsigned remaining, unsigned elapsed) {
      cout << fmt::format("Tick {}\n", remaining);
    });

    miso::connect(timer.signal_expired, [&]() {
      cout << fmt::format("Tock\n");
    });

    int r = (timer_rows.size() * 2) + 2;
    timer_rows.emplace_back(
        window,
        timer_rows.next_grid(),
        timer
      );
    auto& timer_row = timer_rows.back();
    timer_row.Colours(window.Colours());

    miso::connect(timer.signal_expired, [&](){
      five_second_button.deactivate();
    });
  });

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  window.Colours(isolinear::theme::nightgazer_colours);

  bool running = true;
  //SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY));

  while (running) {
    if (drawdebug) {
      grid.draw(window.renderer());
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

      case SDL_MOUSEBUTTONDOWN:
        window.on_pointer_event(pointer::event{ e.button });
        break;

      case SDL_QUIT:
        running = false;
        break;
      }
    }

    window.draw();

    SDL_RenderPresent(window.renderer());
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

