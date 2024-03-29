#include "init.h"
#include "layout.h"
#include "ui.h"
#include "layout.h"


int main(int argc, char* argv[]) {
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::layout::gridfactory gridfactory(
      { 0, 0, window.size().x, window.size().y }, // Display Region
      { 60, 30 }, // Cell Size
      { 6, 6 } // Cell Gutter
  );

  auto& root_grid = gridfactory.root();

  int hthickness = 2;
  int vthickness = 3;
  isolinear::layout::northwest_elbo elbo_layout(root_grid, hthickness + 1, vthickness + 1);
  isolinear::ui::northwest_sweep nwsweep(window, elbo_layout.sweep(), {vthickness, hthickness}, 50, 20 );
  isolinear::ui::vertical_button_bar vbbar(window, elbo_layout.vertical_control());
  isolinear::ui::header_east_bar hbbar(window, elbo_layout.horizontal_control(), "KITCHEN SINK");

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

  isolinear::ui::header_basic label_buttons(button_area.rows(1,2), window, "BUTTONS");
  window.add(&label_buttons);

  isolinear::ui::button single_button(window, button_area.rows(3, 4).west_columns(2), "BUTTON");
  window.add(&single_button);

  isolinear::ui::header_basic label_progress(progress_area.rows(1,2), window, "PROGRESS BARS");
  window.add(&label_progress);

  isolinear::ui::horizontal_progress_bar thin_progress(progress_area.row(3), 40);
  isolinear::ui::horizontal_progress_bar norm_progress(progress_area.rows(4, 5), 50);
  isolinear::ui::horizontal_progress_bar lrge_progress(progress_area.rows(6,8), 60);
  window.add(&thin_progress);
  window.add(&norm_progress);
  window.add(&lrge_progress);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
