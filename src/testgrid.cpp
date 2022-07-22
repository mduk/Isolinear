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
  auto& window = isolinear::new_window();

  isolinear::grid grid(
      { 0, 0, window.size().x, window.size().y }, // Display Region
      { 60, 30 }, // Cell Size
      { 6, 6 } // Cell Gutter
    );

  auto elbo_corner_region = grid.left_columns(4).top_rows(3);
  auto elbo_vertical_region = grid.left_columns(3).bottom_rows(grid.max_rows() - 4);
  auto elbo_horizontal_region = grid.top_rows(2).right_columns(grid.max_columns() - 4);
  auto elbo_content_region = grid.subgrid(5, 4, grid.max_columns(), grid.max_rows());

  isolinear::ui::north_west_sweep nwsweep(
      window, elbo_corner_region, {3, 2}, 50, 20
  );
  window.add(&nwsweep);

  isolinear::ui::vertical_button_bar vbbar(window, elbo_vertical_region);
    vbbar.add_button("Spoon");
    vbbar.add_button("Knife");
    vbbar.add_button("Fork");
  window.add(&vbbar);

  isolinear::ui::horizontal_button_bar hbbar(window, elbo_horizontal_region);
    hbbar.add_button("Spoon");
    hbbar.add_button("Knife");
    hbbar.add_button("Fork");
  window.add(&hbbar);


  auto progressrow = elbo_content_region.rows(1, 2);
  isolinear::ui::horizontal_progress_bar pbar(progressrow);
  pbar.value(50);
  window.add(&pbar);

  isolinear::ui::horizontal_progress_bar volbar_left(elbo_content_region.row(3));
  isolinear::ui::horizontal_progress_bar volbar_right(elbo_content_region.row(4));
  volbar_left.value(50);
  volbar_right.value(50);
  window.add(&volbar_left);
  window.add(&volbar_right);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
