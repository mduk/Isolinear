#include "init.h"
#include "grid.h"
#include "ui.h"
#include "compasslayout.h"


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

  isolinear::layout::northwest_elbo nwelbo(grid, window);

  isolinear::ui::rect vrect(nwelbo.vertical_gutter().bounds());
  window.add(&vrect);

  isolinear::ui::horizontal_rule hrect(nwelbo.horizontal_gutter(), isolinear::compass::north);
  window.add(&hrect);

  isolinear::ui::north_west_sweep nwsweep(
      window, nwelbo.sweep(), {3, 2}, 50, 20
  );
  window.add(&nwsweep);

  isolinear::ui::vertical_button_bar vbbar(window, nwelbo.vertical_control());
    vbbar.add_button("Spoon");
    vbbar.add_button("Knife");
    vbbar.add_button("Fork");
  window.add(&vbbar);

  isolinear::ui::horizontal_button_bar hbbar(window, nwelbo.horizontal_control());
    hbbar.add_button("Spoon");
    hbbar.add_button("Knife");
    hbbar.add_button("Fork");
  window.add(&hbbar);

  isolinear::ui::horizontal_progress_bar pbar(nwelbo.content().rows(1, 2), 40);
  window.add(&pbar);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
