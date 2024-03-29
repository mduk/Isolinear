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
public:
    using game_state_ptr = std::unique_ptr<bool[]>;
    struct generation {
        int alive = 0;
        int dead = 0;
        int alive_delta = 0;
        int dead_delta = 0;
    };

private:
    geometry::vector m_grid_size;
    game_state_ptr m_update;
    game_state_ptr m_display;
    bool m_grid_wrap{true};
    generation m_previous_generation;

public:
    gameoflife(geometry::vector gs)
        : m_grid_size(gs)
        , m_update(std::make_unique<bool[]>(gs.x*gs.y))
        , m_display(std::make_unique<bool[]>(gs.x*gs.y))
    {
      initialise(6);
    }

    int n_cells() const {
      return m_grid_size.x * m_grid_size.y;
    }

    int n_alive_cells() const {
      int alive = 0;
      for (int i=0; i < n_cells(); i++) {
        if (m_display[i]) {
          alive++;
        }
      }
      return alive;
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

    bool wrap() {
      m_grid_wrap = !m_grid_wrap;
      return m_grid_wrap;
    }

    int alive_neighbours_of(const geometry::vector cell) const {
      std::array<std::pair<int,int>, 8> neighbours{{
          {-1, -1},  // Northwest
          { 0, -1},  // North
          {+1, -1},  // Northeast

          {-1,  0},  // West
          {+1,  0},  // East

          {-1, +1},  // Southwest
          { 0, +1},  // South
          {+1, +1},  // Southeast
      }};

      int alive_neighbours = 0;
      for (auto &[ relative_x, relative_y ] : neighbours) {
        int neighbour_x = cell.x + relative_x;
        int neighbour_y = cell.y + relative_y;

        auto [w, h] = m_grid_size;
        if (m_grid_wrap) {
          if (neighbour_x <= 0) { neighbour_x += m_grid_size.x; }
          if (neighbour_x >= w) { neighbour_x -= m_grid_size.x; }
          if (neighbour_y <= 0) { neighbour_y += m_grid_size.y; }
          if (neighbour_y >= h) { neighbour_y -= m_grid_size.y; }
        }
        else {
          if (neighbour_x <= 0) { continue; };
          if (neighbour_x >= w) { continue; };
          if (neighbour_y <= 0) { continue; };
          if (neighbour_y >= h) { continue; };
        }

        if (m_display[xytoi({neighbour_x, neighbour_y})]) {
          alive_neighbours++;
        }
      }

      return alive_neighbours;
    }

    int xytoi(const geometry::vector c) const {
      return (c.y * m_grid_size.x) + c.x;
    }

    geometry::vector size() const {
      return m_grid_size;
    }

    bool cell_state(const geometry::vector c) const {
      return m_display[xytoi(c)];
    }

    generation update() {
      generation gen;
      for (int cy = 0; cy < m_grid_size.y; cy++) {
        for (int cx = 0; cx < m_grid_size.x; cx++) {
          int alive_neighbours = alive_neighbours_of({cx, cy});
          int i = xytoi({cx, cy});
          bool cell_alive = m_display[i];

          if ((cell_alive && (alive_neighbours == 2 || alive_neighbours == 3))
              || (!cell_alive && alive_neighbours == 3)
          ) {
            m_update[i] = true;
            gen.alive++;
            continue;
          }

          m_update[i] = false;
          gen.dead++;
        }
      }

      m_display.swap(m_update);

      gen.alive_delta = m_previous_generation.alive - gen.alive;
      gen.dead_delta  = m_previous_generation.dead  - gen.dead;

      m_previous_generation = gen;
      return gen;
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
    miso::signal<gameoflife::generation> signal_step;

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

    bool wrap() {
      return m_game.wrap();
    }

    const bool pause() {
      m_pause = !m_pause;
      return m_pause;
    }

    void step() {
      emit signal_step(m_game.update());
    }

    void update() {
      if (m_pause) {
        return;
      }
      step();
    }

    void on_pointer_event(const isolinear::event::pointer event) {
      m_hover_cell = m_game_grid.cell_at(event.position());
    }

    void draw(SDL_Renderer* renderer) const {
      auto [ grid_x, grid_y ] = m_game.size();
      for (int cy = 0; cy < grid_y; cy++) {
        for (int cx = 0; cx < grid_x; cx++) {

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

  int sidebar_thickness = 4;

  auto& root_grid = gridfactory.root();
  layout::grid content_grid = root_grid.east_columns(root_grid.max_columns() - sidebar_thickness);
  layout::grid control_grid = root_grid.west_columns(sidebar_thickness);

  layout::grid graph_grid       = content_grid.rows(1, 6);
  layout::grid graph_ctrl_grid  = control_grid.rows(1, 5);
  layout::grid graph_sweep_grid = control_grid.rows(6, 7);

  layout::grid divider_grid = content_grid.rows(7, 8);

  layout::grid life_grid       = content_grid.rows(9, content_grid.max_rows());
  layout::grid life_sweep_grid = control_grid.rows(8, 9);
  layout::grid life_ctrl_grid  = control_grid.rows(10, control_grid.max_rows());

  ui::vertical_button_bar graph_buttons(window, graph_ctrl_grid.west_columns(3)); window.add(&graph_buttons);
    graph_buttons.add_button("01-2287");
    graph_buttons.add_button("01-9232");

  ui::northwest_sweep nwsweep(window, life_sweep_grid, {3, 1}, 50, 20 ); window.add(&nwsweep);
  ui::southwest_sweep swsweep(window, graph_sweep_grid, {3, 1}, 50, 20); window.add(&swsweep);
  ui::header_east_bar header_bar(window, divider_grid, "GAME OF LIFE"); window.add(&header_bar);

  ui::vertical_button_bar vbbar(window, life_ctrl_grid.west_columns(3)); window.add(&vbbar);
    ui::button &randomise_btn = vbbar.add_button("RANDOMISE");
    ui::button &mutate_btn = vbbar.add_button("MUTATE");
    ui::button &pause_btn = vbbar.add_button("PAUSE");
    ui::button &step_btn = vbbar.add_button("STEP");
    ui::button &wrap_btn = vbbar.add_button("WRAP");

  isogameoflife gol(life_grid);
  window.add(&gol);

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

  step_btn.disable();
  miso::connect(step_btn.signal_press, [&](){
      gol.step();
  });

  wrap_btn.activate();
  miso::connect(wrap_btn.signal_press, [&](){
      wrap_btn.active(gol.wrap());
  });

  miso::connect(gol.signal_step, [&](gameoflife::generation gen){
    header_bar.label(fmt::format(
        "{} alive ({}), {} dead ({})",
        gen.alive, gen.alive_delta, gen.dead, gen.dead_delta
    ));
  });

  while (isolinear::loop()) {
    gol.update();
    //SDL_Delay(100);
  }

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
