#include <asio.hpp>
#include <thread>
#include <iostream>
#include <fmt/core.h>
#include <SDL2/SDL.h>

#include "miso.h"

#include "init.h"
#include "theme.h"
#include "layout.h"
#include "grid.h"
#include "mpd.h"
#include "event.h"


int main(int argc, char* argv[])
{
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::grid grid(
      { 0, 0, window.size().x, window.size().y },
      { window.button_font().Height() * 2, window.button_font().Height() },
      { 10, 10 }
    );


  mpdxx::client mpdc(isolinear::io_context, "localhost", "6600");

  miso::connect(mpdc.signal_status, [&mpdc](mpdxx::status status){
    cout << "Status:\n";
    cout << status << "\n";
  });

  isompd::frame mpdframe(grid, window, mpdc);
  window.add(&mpdframe);

  miso::connect(mpdframe.signal_view_change, [&](std::string from_view, std::string to_view){
    cout << fmt::format("View changed from {} to {}\n", from_view, to_view);
    if (to_view == mpdframe.V_QUEUE     ) { mpdc.RequestQueue();       }
    if (to_view == mpdframe.V_NOWPLAYING) { mpdc.RequestCurrentSong(); }
    if (to_view == mpdframe.V_BROWSE    ) { mpdc.RequestArtistList();  }
  });

  window.colours(isolinear::theme::nightgazer_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  SDL_ShowCursor(true);
  mpdc.RequestStatus();

  while (running) {

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: running = false; break;

            case 's': mpdc.RequestStatus(); break;

            case 'd': window.colours(isolinear::theme::debug_colours     ); break;
            case 'n': window.colours(isolinear::theme::nightgazer_colours); break;

            case 'a': mpdframe.SwitchView(mpdframe.V_BROWSE    ); break;
            case 'c': mpdframe.SwitchView(mpdframe.V_NOWPLAYING); break;
            case 'q': mpdframe.SwitchView(mpdframe.V_QUEUE     ); break;
            case 'p': mpdframe.SwitchView(mpdframe.V_PLAYER    ); break;
          }
          break;

        case SDL_MOUSEMOTION:
          window.on_pointer_event(isolinear::event::pointer(e.motion));
          break;

        case SDL_MOUSEBUTTONDOWN:
          window.on_pointer_event(isolinear::event::pointer(e.button));
          break;

        case SDL_QUIT:
          running = false;
          break;
      }
    }

    window.render();
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
