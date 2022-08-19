#include "init.h"
#include "ui.h"
#include <filesystem>

class frame_hbar : public isolinear::ui::control {

protected:
    isolinear::display::window& m_window;
    isolinear::layout::horizontal_row m_layout;
    std::string m_header;
    isolinear::compass m_header_alignment{isolinear::west};
    std::list<isolinear::ui::button> m_buttons;

public:
    frame_hbar(isolinear::display::window& w, isolinear::layout::grid g)
      : isolinear::ui::control(g)
      , m_window(w)
      , m_layout(g)
    {}

public:
    isolinear::layout::horizontal_row& layout() {
      return m_layout;
    }

    std::string header(std::string h) {
      m_header = h;
      return m_header;
    }

    isolinear::ui::button& add_button_east(std::string label, uint width) {
      auto& button = m_buttons.emplace_back(m_window, m_layout.allocate_east(width), label);
      register_child(&button);
      return button;
    }

    isolinear::ui::button& add_button_west(std::string label, uint width) {
      auto& button = m_buttons.emplace_back(m_window, m_layout.allocate_west(width), label);
      register_child(&button);
      return button;
    }

    void draw(SDL_Renderer *renderer) const override {
      control::draw(renderer);
      for (auto& btn : m_buttons) btn.draw(renderer);

      auto remainder = m_layout.remainder().bounds();
      if (m_header.empty()) {
        remainder.fill(renderer, colours().frame);
      }
      else {
        m_window.header_font().RenderText(renderer, remainder, m_header_alignment, m_header);
      }
    }
};

class frame : public isolinear::ui::control {

protected:
    isolinear::layout::compass m_layout;

    frame_hbar m_north;
    isolinear::layout::vertical_row m_east;
    frame_hbar m_south;
    isolinear::layout::vertical_row m_west;

    isolinear::ui::north_west_sweep m_nw_sweep;
    isolinear::ui::south_west_sweep m_sw_sweep;
    std::list<isolinear::ui::button> m_buttons;

public:
    frame(isolinear::display::window& w, isolinear::layout::grid g, const std::string& h)
        : isolinear::ui::control(g)
        , m_layout(m_grid, 2, 2, 1, 2, {0}, {0}, {3,2}, {3,3})

        , m_north(w, m_layout.north())
        , m_east(m_layout.east())
        , m_south(w, m_layout.south())
        , m_west(m_layout.west())

        , m_nw_sweep(w, m_layout.northwest(),{2,2},20,10)
        , m_sw_sweep(w, m_layout.southwest(),{2,1},20,10)
    {
      register_child(&m_north);
      register_child(&m_south);
      register_child(&m_nw_sweep);
      register_child(&m_sw_sweep);

      for (auto& button : m_buttons) register_child(&button);
    }

    frame_hbar& north() { return m_north; }
    frame_hbar& south() { return m_south; }

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

  std::string path = std::getenv("DEV_DIR");

  auto& root = gridfactory.root();
  isolinear::layout::horizontal_row hrow(root);

  isolinear::ui::header_east_bar info_frame(window, hrow.allocate_east(10).rows(1,2), "FILE DETAIL");
  window.add(&info_frame);

  frame list_frame(window, hrow.remainder(), path);
  window.add(&list_frame);

  list_frame.north().header(path);
  list_frame.north().add_button_east("NEW DIR", 2);
  list_frame.north().add_button_west("PARENT DIR", 3);

  isolinear::layout::vertical_row vrow(list_frame.centre());

  std::list<isolinear::ui::header_basic> headers;
  std::list<isolinear::ui::button> buttons;
  std::list<isolinear::ui::vertical_rule> vrules;

  for (const auto& entry : fs::directory_iterator(path)) {

//    if (!fs::is_directory(entry.path())) {
//      continue;
//    }

    if (entry.path().filename().string()[0] == '.') {
      continue;
    }

    isolinear::layout::horizontal_row hrow(vrow.allocate_north(2));

    auto status = fs::status(entry.path());
    fs::perms perms = status.permissions();

    headers.emplace_back(window, hrow.remainder(), compass::west, entry.path().filename());
  }

  for (auto& header : headers) window.add(&header);
  for (auto& button : buttons) window.add(&button);
  for (auto& vrule : vrules) window.add(&vrule);

  while (isolinear::loop());

  work_guard.reset();
  isolinear::shutdown();
  return 0;
}

/*
    auto& others_execute = buttons.emplace_back(window, hrow.allocate_east(1), "X");
    others_execute.active((perms & fs::perms::others_exec) != fs::perms::none);
    auto& others_write = buttons.emplace_back(window, hrow.allocate_east(1), "W");
    others_write.active((perms & fs::perms::others_write) != fs::perms::none);
    auto& others_read = buttons.emplace_back(window, hrow.allocate_east(1), "R");
    others_read.active((perms & fs::perms::others_read) != fs::perms::none);

    vrules.emplace_back(hrow.allocate_east(1), compass::centre);

    auto& owner_execute = buttons.emplace_back(window, hrow.allocate_east(1), "X");
    owner_execute.active((perms & fs::perms::owner_exec) != fs::perms::none);
    auto& owner_write = buttons.emplace_back(window, hrow.allocate_east(1), "W");
    owner_write.active((perms & fs::perms::owner_write) != fs::perms::none);
    auto& owner_read = buttons.emplace_back(window, hrow.allocate_east(1), "R");
    owner_read.active((perms & fs::perms::owner_read) != fs::perms::none);

    vrules.emplace_back(hrow.allocate_east(1), compass::centre);

    auto& group_execute = buttons.emplace_back(window, hrow.allocate_east(1), "X");
    group_execute.active((perms & fs::perms::group_exec) != fs::perms::none);
    auto& group_write = buttons.emplace_back(window, hrow.allocate_east(1), "W");
    group_write.active((perms & fs::perms::group_write) != fs::perms::none);
    auto& group_read = buttons.emplace_back(window, hrow.allocate_east(1), "R");
    group_read.active((perms & fs::perms::group_read) != fs::perms::none);
 */