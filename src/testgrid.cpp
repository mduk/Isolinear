#include "init.h"
#include "grid.h"
#include "ui.h"
#include "layout.h"


int main(int argc, char* argv[]) {
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::grid grid(
      { 0, 0, window.size().x, window.size().y }, // Display Region
      { 60, 30 }, // Cell Size
      { 6, 6 } // Cell Gutter
    );

  int hthickness = 2;
  int vthickness = 3;
  isolinear::layout::northwest_elbo elbo_layout(grid, hthickness+1, vthickness+1);
  isolinear::ui::north_west_sweep nwsweep( window, elbo_layout.sweep(), {vthickness, hthickness}, 50, 20 );
  isolinear::ui::vertical_button_bar vbbar(window, elbo_layout.vertical_control());
  isolinear::ui::horizontal_button_bar hbbar(window, elbo_layout.horizontal_control());

  window.add(&nwsweep);
  window.add(&vbbar);
  window.add(&hbbar);

  vbbar.add_button("Spoon");
  vbbar.add_button("Knife");
  vbbar.add_button("Fork");
  hbbar.add_button("Spoon");
  hbbar.add_button("Knife");
  hbbar.add_button("Fork");

  auto button_area = elbo_layout.content().rows(1,6);
  auto progress_area = elbo_layout.content().rows(7,12);

  isolinear::ui::button single_button(window, button_area.rows(1,2).left_columns(2), "BUTTON");
  window.add(&single_button);

  isolinear::ui::horizontal_progress_bar pbar(progress_area.rows(1, 2), 40);
  window.add(&pbar);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
