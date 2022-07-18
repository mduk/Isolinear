#include <sstream>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "init.h"
#include "geometry.h"
#include "grid.h"
#include "display.h"
#include "ui.h"


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

  int cell_scale  = 30;

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

  isolinear::grid grid(
      { 0, 0, display_size.x, display_size.y }, // Display Region
      { cell_scale*2, cell_scale }, // Cell Size
      { 6, 6 } // Cell Gutter
    );

  auto sweepgrid = grid.left_columns(4).top_rows(3);

  isolinear::ui::north_west_sweep nwsweep(
      window,
      sweepgrid,
      {3, 2},
      50,
      20
    );
  window.add(&nwsweep);

  auto leftcol = grid.left_columns(3);
  auto vbbgrid = leftcol.bottom_rows(leftcol.max_rows() - 4);
  isolinear::ui::vertical_button_bar vbbar(window, vbbgrid);
    vbbar.add_button("Spoon");
    vbbar.add_button("Knife");
    vbbar.add_button("Fork");
  window.add(&vbbar);

  auto toprow = grid.top_rows(2);
  auto hbbgrid = toprow.right_columns(toprow.max_columns() - 4);
  isolinear::ui::horizontal_button_bar hbbar(window, hbbgrid);
    hbbar.add_button("Spoon");
    hbbar.add_button("Knife");
    hbbar.add_button("Fork");
    window.add(&hbbar);

  printf("LOOP\n");
  while (running) {

    window.colours(isolinear::theme::nightgazer_colours);

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN:
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
              running = false;
              break;

            case SDLK_UP:
              cell_scale--;
              grid.set_cell_size({ cell_scale*2, cell_scale });
             break;

            case SDLK_DOWN:
              cell_scale++;
              grid.set_cell_size({cell_scale*2, cell_scale});
              break;
          }
          break;

        case SDL_MOUSEMOTION:
          window.on_pointer_event(pointer::event(e.motion));
          break;

        case SDL_QUIT:
          running = false;
          break;
      }
    }


    grid.draw(window.renderer());
    grid.left_columns(2).draw(window.renderer());
    grid.right_columns(2).draw(window.renderer());
    grid.centre_columns(8,8).draw(window.renderer());
    grid.top_rows(1).draw(window.renderer());
    grid.bottom_rows(1).draw(window.renderer());
    grid.centre_rows(8,8).draw(window.renderer());

    window.draw();

    SDL_RenderPresent(window.renderer());
  } // while (running)

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
