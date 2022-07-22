#include <sstream>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "init.h"
#include "display.h"
#include "ui.h"


int main(int argc, char *argv[]) {
  namespace geometry = isolinear::geometry;
  namespace pointer = isolinear::pointer;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window1 = isolinear::new_window({100, 100}, {500, 500});
  auto& window2 = isolinear::new_window({700, 100}, {500, 500});

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  isolinear::ui::rect rect1{{ 10, 10, 150, 50 }};
  window1.add(&rect1);

  isolinear::ui::rect rect2{{ 50, 50, 150, 50 }};
  window2.add(&rect2);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
