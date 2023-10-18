#include "init.h"
#include "layout.h"
#include "ui.h"
#include "layout.h"
#include "fmt/core.h"

using isolinear::geometry::region;
namespace layout = isolinear::layout;
namespace geometry = isolinear::geometry;
namespace theme = isolinear::theme;
namespace ui = isolinear::ui;

using game_state = std::vector<bool>;

class gameoflife : public isolinear::ui::control {
protected:
    geometry::vector m_bigness{67,39};
    layout::grid m_game_grid;
    game_state m_state;
    bool m_paused = false;
    geometry::vector m_focused_cell{0};

public:
    miso::signal<bool> signal_paused;
    miso::signal<int, int, int> signal_mouse;

public:
    gameoflife(isolinear::layout::grid g)
        : control(g)
        , m_game_grid(g.bounds(), {25}, {5}, m_bigness, {0})
        {
      initialise();
    }

    void initialise() {
      m_state.clear();
      auto cell_count = m_game_grid.cell_count();
      for (uint16_t index = 1; index <= cell_count; index++) {
        m_state.push_back(rand() % 6 == 0);
      }
    }

    int alive_neighbours_of(geometry::vector cell, game_state &state) const {
      int alive = 0;
      for (int xo = -1; xo <= 1; xo++) {
        for (int yo = -1; yo <= 1; yo++) {
          if (xo == 0 && yo == 0) {
            continue;
          }
          auto i = xytoi( cell.x + xo, cell.y + yo );
          if (state[i]) {
            alive++;
          }
        }
      }
      return alive;
    }

    bool cell_update(geometry::vector cell, game_state &read_state) {
      auto cell_alive = read_state[xytoi(cell)];
      auto neighbours = alive_neighbours_of(cell, read_state);
      if (cell_alive && (neighbours < 2 || neighbours > 3)) {
        return false;
      }
      if (!cell_alive && neighbours == 3) {
        return true;
      }
      return cell_alive;
    }

    void update() {
      if (m_paused) {
        return;
      }

      do_update();
    }

    void do_update() {
      auto cols = m_game_grid.max_columns();
      auto rows = m_game_grid.max_columns();

      game_state temp;
      auto cell_count = m_game_grid.cell_count();
      for (uint16_t index = 1; index <= cell_count; index++) {
        temp.push_back(0);
      }
      for (int x = 0; x < cols; x++) {
        for (int y = 0; y < rows; y++) {
          geometry::vector cell_ref{x, y};
          temp[xytoi(x, y)] = cell_update(cell_ref, m_state);
        }
      }
      m_state = temp;
    }

    int xytoi(int x, int y) const {
      return (y * m_game_grid.max_columns()) + x;
    }
    int xytoi(geometry::vector position) const {
      return xytoi(position.x, position.y);
    }

    void draw(SDL_Renderer* renderer) const {
      m_game_grid.bounds().fill(renderer, 0xff333333);

      auto focused_cell_i = xytoi(m_focused_cell);

      for (int x = 1; x <= m_game_grid.max_columns(); x++) {
        for (int y = 1; y <= m_game_grid.max_rows(); y++) {
          theme::colour cell_colour{0xff000000};
          auto i = xytoi(x,y);
          if (m_state[i]) {
            cell_colour = 0xffffffff;
          }

          if (i == focused_cell_i) {
            m_game_grid.cell(x, y).fill(renderer, isolinear::theme::nightgazer_colours.active);
            m_game_grid.cell(x, y).shrink(5).fill(renderer, cell_colour);
          }
          else {
            m_game_grid.cell(x, y).fill(renderer, cell_colour);
          }
        }
      }
      SDL_Delay(50);
    }

    virtual void on_mouse_down(isolinear::event::pointer event) {
      auto clicked_cell = m_game_grid.cell_at(event.position());

      if (clicked_cell.x == m_focused_cell.x && clicked_cell.y == m_focused_cell.y) {
        auto i = xytoi(m_focused_cell);
        m_state[i] = !m_state[i];
      }
      else {
        m_focused_cell = clicked_cell;
      }

      auto alive = alive_neighbours_of(m_focused_cell, m_state);
      emit signal_mouse(m_focused_cell.x, m_focused_cell.y, alive);
    }

    void pause() {
      m_paused = !m_paused;
      emit signal_paused(m_paused);
    }

    void step() {
      if (!m_paused) {
        return;
      }

      do_update();
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

  isolinear::layout::grid content_area = elbo_layout.content();
  gameoflife gol(content_area);
  window.add(&gol);

  ui::button &randomise_btn = vbbar.add_button("RANDOMISE");
  ui::button &pause_btn = vbbar.add_button("PAUSE");
  ui::button &step_btn = vbbar.add_button("STEP");
  step_btn.disable();

  miso::connect(gol.signal_mouse, [&hbbar](int x, int y, int alive){
    hbbar.label(fmt::format("{}x{} ({})", x, y, alive));
  });
  miso::connect(pause_btn.signal_press, [&gol](){ gol.pause(); });
  miso::connect(randomise_btn.signal_press, [&gol](){ gol.initialise(); });
  miso::connect(step_btn.signal_press, [&gol](){ gol.step(); });

  miso::connect(gol.signal_paused, [&step_btn](bool paused){
    if (paused) {
      step_btn.enable();
    }
    else {
      step_btn.disable();
    }
  });

  while (isolinear::loop()) {
    gol.update();
  };

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
