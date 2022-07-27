#include <asio.hpp>
#include <thread>
#include <iostream>
#include <fmt/core.h>
#include <SDL2/SDL.h>

#include "miso.h"

#include "init.h"
#include "theme.h"
#include "layout.h"
#include "layout.h"
#include "mpd.h"
#include "event.h"


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::layout::gridfactory gridfactory(
      { 0, 0, window.size().x, window.size().y },
      { window.button_font().Height() * 2, window.button_font().Height() },
      { 10, 10 }
    );

  auto& grid = gridfactory.root();

  mpdxx::client mpdc(isolinear::io_context, "localhost", "6600");

  miso::connect(mpdc.signal_status, [&mpdc](mpdxx::status status){
    cout << "Status:\n";
    cout << status << "\n";
  });

  isompd::frame mpdframe(grid, window, mpdc);
  window.add(&mpdframe);

  miso::connect(mpdframe.signal_view_change, [&](std::string from_view, std::string to_view){
    cout << fmt::format("View changed from {} to {}\n", from_view, to_view);
  });

  window.colours(isolinear::theme::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  SDL_ShowCursor(true);
  mpdc.RequestStatus();

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
