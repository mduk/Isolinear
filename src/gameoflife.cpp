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

class gameoflife {
private:
    using game_state_ptr = std::unique_ptr<bool[]>;

    geometry::vector m_grid_size;
    game_state_ptr m_update;
    game_state_ptr m_display;

public:
    gameoflife(geometry::vector gs)
        : m_grid_size(gs)
        , m_update(std::make_unique<bool[]>(gs.x*gs.y))
        , m_display(std::make_unique<bool[]>(gs.x*gs.y))
    {
      initialise(6);
    }

    int n_cells() {
      return m_grid_size.x * m_grid_size.y;
    }

    void initialise(const int factor) {
      for (int i = 0; i < n_cells(); i++) {
        m_display[i] = rand() % factor == 0;
        m_update[i] = 0;
      }
    }

    void mutate(const int factor) {
      for (int i = 0; i < n_cells(); i++) {
        if (!m_display[i]) {
          m_display[i] = rand() % factor == 0;
        }
      }
    }

    geometry::vector neighbour(geometry::vector cell, geometry::vector offset, geometry::vector gs) {
      int x = cell.x + offset.x;
      int y = cell.y + offset.y;

      if (x < 0) { x += gs.x; }
      if (x > gs.x) { x -= gs.x; }
      if (y < 0) { y += gs.y; }
      if (y > gs.y) { y -= gs.y; }

      return {x, y};
    }

    std::array<geometry::vector, 8> neighbours_of(geometry::vector cell, geometry::vector gs) {
      return {
          neighbour(cell, {-1, -1}, gs),  // Northwest
          neighbour(cell, { 0, -1}, gs),  // North
          neighbour(cell, {+1, -1}, gs),  // Northeast

          neighbour(cell, {-1,  0}, gs),  // West
          neighbour(cell, {+1,  0}, gs),  // East

          neighbour(cell, {-1, +1}, gs),  // Southwest
          neighbour(cell, { 0, +1}, gs),  // South
          neighbour(cell, {+1, +1}, gs),  // Southeast
      };
    }

    int xytoi(geometry::vector c, int n_columns) const {
      return (c.y * n_columns) + c.x;
    }

    geometry::vector size() const {
      return m_grid_size;
    }

    bool cell_state(geometry::vector c) const {
      return m_display[xytoi(c, m_grid_size.x)];
    }

    void update() {
      for (int cy = 0; cy < m_grid_size.y; cy++) {
        for (int cx = 0; cx < m_grid_size.x; cx++) {
          int alive = 0;
          for (auto neighbour : neighbours_of({cx, cy}, m_grid_size)) {
            if (m_display[xytoi(neighbour, m_grid_size.x)]) {
              alive++;
            }
          }

          int i = xytoi({cx, cy}, m_grid_size.x);
          bool cell_alive = m_display[i];

          if (cell_alive && (alive == 2 || alive == 3)) {
            m_update[i] = true;
            continue;
          }

          if (!cell_alive && alive == 3) {
            m_update[i] = true;
            continue;
          }

          m_update[i] = false;
        }
      }

      m_display.swap(m_update);
    }
};

class isogameoflife : public isolinear::ui::control {
protected:
    std::size_t m_cell_size;
    gameoflife m_game;
    layout::grid m_game_grid;
    geometry::vector m_hover_cell{0};
    bool m_pause{false};

public:
    isogameoflife(isolinear::layout::grid g)
    : control(g)
    , m_cell_size(5)
    , m_game({
        static_cast<int>(floor(g.bounds().W()/m_cell_size)),
        static_cast<int>(floor(g.bounds().H()/m_cell_size))
    })
    , m_game_grid(g.bounds(), {static_cast<int>(m_cell_size)}, 4, m_game.size(), 0)
    { }

    void initialise(const int factor) {
      m_game.initialise(factor);
    }

    void mutate(const int factor) {
      m_game.mutate(factor);
    }

    const bool pause() {
      m_pause = !m_pause;
      return m_pause;
    }

    void step() {
      m_game.update();
    }

    void update() {
      if (m_pause) {
        return;
      }
      m_game.update();
    }

    void on_pointer_event(const isolinear::event::pointer event) {
      m_hover_cell = m_game_grid.cell_at(event.position());
    }

    void draw(SDL_Renderer* renderer) const {
      auto grid_size = m_game.size();
      for (int cy = 0; cy < grid_size.y; cy++) {
        for (int cx = 0; cx < grid_size.x; cx++) {

          int cell_colour = (
              m_game.cell_state({cx, cy})
              ? 0xffffffff
              : 0xff000000
          );

          geometry::vector cell_coord{cx, cy};
          if (cell_coord == m_hover_cell) {
            cell_colour = 0xff0000ff;
          }

          auto cell_region = m_game_grid.cell(cx, cy);

          boxColor(
              renderer,
              cell_region.near_x(), cell_region.near_y(),
              cell_region.far_x(), cell_region.far_y(),
              cell_colour
          );
        }
      }
    }
};

int main(int argc, char* argv[]) {
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto display = isolinear::display::detect_displays().front();
  auto& window = isolinear::new_window(
      {display.X(), display.Y()},
      {display.W(), display.H()}
      );

  isolinear::layout::gridfactory gridfactory(
      { 0, 0, window.size().x, window.size().y }, // Display Region
      { 60, 30 }, // Cell Size
      { 6, 6 } // Cell Gutter
  );

  auto& root_grid = gridfactory.root();

  int hthickness = 2;
  int vthickness = 2;
  isolinear::layout::northwest_elbo elbo_layout(root_grid, hthickness + 1, vthickness + 1);
  isolinear::ui::northwest_sweep nwsweep(window, elbo_layout.sweep(), {vthickness, hthickness}, 50, 20 );
  isolinear::ui::vertical_button_bar vbbar(window, elbo_layout.vertical_control());
  isolinear::ui::header_east_bar hbbar(window, elbo_layout.horizontal_control(), "GAME OF LIFE");

  window.add(&nwsweep);
  window.add(&vbbar);
  window.add(&hbbar);

  layout::grid content_area = elbo_layout.content();

  isogameoflife gol(content_area);
  window.add(&gol);
/*
  ui::vertical_rule divider_rule(content_area.column(21), compass::centre);
  window.add(&divider_rule);

  ui::rect side_block(content_area.columns(22, content_area.max_columns()));
  window.add(&side_block);
*/

  ui::button &randomise_btn = vbbar.add_button("RANDOMISE");
  ui::button &mutate_btn = vbbar.add_button("MUTATE");
  ui::button &pause_btn = vbbar.add_button("PAUSE");
  ui::button &step_btn = vbbar.add_button("STEP");
  step_btn.disable();

  miso::connect(randomise_btn.signal_press, [&](){
    gol.initialise(12);
  });

  miso::connect(mutate_btn.signal_press, [&](){
    gol.mutate(200);
  });

  miso::connect(pause_btn.signal_press, [&](){
    if (gol.pause()) {
      step_btn.enable();
      pause_btn.activate();
    }
    else {
      step_btn.disable();
      pause_btn.deactivate();
    }
  });

  miso::connect(step_btn.signal_press, [&](){
    gol.step();
  });

  while (isolinear::loop()) {
    gol.update();
 //   SDL_Delay(50);
  };

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
