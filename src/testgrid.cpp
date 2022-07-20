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

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto window = isolinear::new_window();

  int cell_scale  = 30;

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

  isolinear::grid grid(
      { 0, 0, window.size().x, window.size().y }, // Display Region
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

  auto leftcol = grid.left_columns(3).bottom_rows(grid.max_rows() - 4);
  auto toprow = grid.top_rows(2).right_columns(grid.max_columns() - 4);
  auto content_area = grid.subgrid(5,4, grid.max_columns(), grid.max_rows());

  isolinear::ui::vertical_button_bar vbbar(window, leftcol);
    vbbar.add_button("Spoon");
    vbbar.add_button("Knife");
    vbbar.add_button("Fork");
  window.add(&vbbar);

  isolinear::ui::horizontal_button_bar hbbar(window, toprow);
    hbbar.add_button("Spoon");
    hbbar.add_button("Knife");
    hbbar.add_button("Fork");
  window.add(&hbbar);


  auto progressrow = content_area.rows(1,2);
  isolinear::ui::horizontal_progress_bar pbar(progressrow);
  pbar.val(50);
  window.add(&pbar);

  isolinear::ui::horizontal_progress_bar volbar_left(content_area.row(3));
  isolinear::ui::horizontal_progress_bar volbar_right(content_area.row(4));
  volbar_left.val(50);
  volbar_right.val(50);
  window.add(&volbar_left);
  window.add(&volbar_right);

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
