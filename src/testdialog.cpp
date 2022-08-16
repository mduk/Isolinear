#include "init.h"
#include "ui.h"
#include <filesystem>

class debugcompass
    : public isolinear::layout::compass
    , public isolinear::ui::control
    {

  public:
    debugcompass(isolinear::layout::grid& g,
                 int n, int e, int s, int w,
                 isolinear::geometry::vector ne,
                 isolinear::geometry::vector se,
                 isolinear::geometry::vector sw,
                 isolinear::geometry::vector nw)
      : isolinear::layout::compass(g, n, e, s, w, ne, se, sw, nw)
      , isolinear::ui::control(g)
      {}

  private:
    void draw(SDL_Renderer *renderer) const override {
      north().bounds().fill(renderer, 0x66ffffff);

      east().bounds().fill(renderer, 0x66ffffff);
      east().draw(renderer);

      south().bounds().fill(renderer, 0x66ffffff);
      west().bounds().fill(renderer, 0x66ffffff);

      centre().draw(renderer);

      northeast().bounds().fill(renderer, 0x66ffffff);
      northeast().draw(renderer);

      southeast().bounds().fill(renderer, 0x66ffffff);
      southwest().bounds().fill(renderer, 0x66ffffff);
      northwest().bounds().fill(renderer, 0x66ffffff);
    }
};

class dialog : public isolinear::ui::control {

  protected:
    debugcompass m_layout;
    isolinear::ui::north_east_sweep m_ne_sweep;
    isolinear::ui::north_west_sweep m_nw_sweep;
    isolinear::ui::south_east_sweep m_se_sweep;
    isolinear::ui::south_west_sweep m_sw_sweep;
    isolinear::ui::horizontal_rule m_n_rule;
    isolinear::ui::horizontal_rule m_s_rule;
    isolinear::ui::vertical_rule m_w_rule;
    isolinear::ui::header_basic m_n_header;
    isolinear::ui::button m_button_ok;
    isolinear::ui::button m_button_cancel;
    isolinear::ui::rect m_button_fill;

public:
    dialog(isolinear::display::window& w, isolinear::layout::grid& g)
      : isolinear::ui::control(g)
      , m_layout(m_grid, 1, 2, 1, 2, {3,3}, {3,2}, {2,2}, {2,2})
      , m_ne_sweep(w, m_layout.northeast(),{2,1},20,10)
      , m_se_sweep(w, m_layout.southeast(),{2,1},20,10)
      , m_sw_sweep(w, m_layout.southwest(),{1,1},20,10)
      , m_nw_sweep(w, m_layout.northwest(),{1,1},20,10)
      , m_n_rule(m_layout.north(), isolinear::compass::north)
      , m_s_rule(m_layout.south(), isolinear::compass::south)
      , m_w_rule(m_layout.west(), isolinear::compass::west)
      , m_button_ok(w, m_layout.east().top_rows(m_layout.east().max_rows() / 4), "CONFIRM")
      , m_button_fill(m_layout.east().top_rows(m_layout.east().max_rows() / 2).bottom_rows(m_layout.east().max_rows()/4))
      , m_button_cancel(w, m_layout.east().bottom_rows(m_layout.east().max_rows() / 2), "CANCEL")
      , m_n_header(w, m_layout.north(), isolinear::compass::northeast, "CONFIRM DELETE")
    {
      register_child(&m_ne_sweep);
      register_child(&m_nw_sweep);
      register_child(&m_se_sweep);
      register_child(&m_sw_sweep);
      register_child(&m_n_rule);
      register_child(&m_s_rule);
      register_child(&m_w_rule);
      register_child(&m_n_header);
      //register_child(&m_layout);
      register_child(&m_button_fill);
      register_child(&m_button_ok);
      register_child(&m_button_cancel);
    }

    isolinear::layout::grid centre() const {
      return m_layout.centre();
    }

};


int main(int argc, char* argv[]) {
  namespace fs = std::filesystem;
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::layout::gridfactory gridfactory(window.region(), {60,30}, {6,6});
  auto& grid = gridfactory.subgrid(6,6,-6,-6);
  dialog mydialog(window, grid);
  window.add(&mydialog);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}
