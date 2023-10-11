#include "init.h"
#include "layout.h"
#include "ui.h"
#include "layout.h"
#include "fmt/core.h"

using isolinear::geometry::region;

class gameoflife : public isolinear::ui::control {
protected:
    uint16_t m_cell_size = 10;
    uint16_t m_columns = 0;
    uint16_t m_rows = 0;
    uint16_t m_length = 0;
    std::vector<bool> m_state{};
    bool m_paused = false;

public:

    gameoflife(isolinear::layout::grid g)
        : control(g)
        , m_columns(g.bounds().H() / m_cell_size)
        , m_rows(g.bounds().W() / m_cell_size)
        , m_length(m_columns * m_rows)
        {
      for (uint16_t index = 0; index < m_length; index++) {
          m_state.push_back(rand() % 2 == 0);
      }
    }

    region cell_region(uint16_t index) const {
      region bounds = m_grid.bounds();
      return region{
        bounds.near_x() + (m_cell_size * (index / m_columns)),
        bounds.near_y() + (m_cell_size * (index % m_columns)),
        m_cell_size,
      };
    }

    void update() {
      if (m_paused) return;

      m_state.clear();
      for (uint16_t index = 0; index < m_length; index++) {
        m_state.push_back(rand() % 2 == 0);
      }
    }

    void draw(SDL_Renderer* renderer) const {
      for (uint16_t index = 0; index < m_length; index++) {
        uint32_t colour = m_state[index] ? 0xffffffff : 0xff000000;
        cell_region(index).fill(renderer, colour);
      }
    }

    void pause() {
      m_paused = !m_paused;
    }
};

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
  isolinear::ui::north_west_sweep nwsweep( window, elbo_layout.sweep(), {vthickness, hthickness}, 50, 20 );
  isolinear::ui::vertical_button_bar vbbar(window, elbo_layout.vertical_control());
  isolinear::ui::header_east_bar hbbar(window, elbo_layout.horizontal_control(), "GAME OF LIFE");

  window.add(&nwsweep);
  window.add(&vbbar);
  window.add(&hbbar);

  isolinear::ui::button &pause_btn = vbbar.add_button("PAUSE");

  isolinear::layout::grid content_area = elbo_layout.content();
  gameoflife gol(content_area);
  window.add(&gol);

  miso::connect(pause_btn.signal_press, [&gol](){
    gol.pause();
  });

  while (isolinear::loop()) {
    gol.update();
  };

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
