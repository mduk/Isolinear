#include "init.h"


int main(int argc, char* argv[]) {
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window({100,100}, {500,500});
  auto& window2 = isolinear::new_window({700, 100}, {500,500});

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
