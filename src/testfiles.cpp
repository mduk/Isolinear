#include "init.h"
#include "ui.h"
#include <filesystem>


class frame : public isolinear::ui::control {

protected:
    isolinear::layout::compass m_layout;
    isolinear::layout::horizontal_row m_north;
    isolinear::layout::vertical_row m_east;

    isolinear::ui::north_east_sweep m_ne_sweep;
    isolinear::ui::south_east_sweep m_se_sweep;

    isolinear::ui::horizontal_rule m_s_rule;


    isolinear::ui::header_basic m_n_header;

    std::list<isolinear::ui::rect> m_rects;
    std::list<isolinear::ui::button> m_buttons;

public:
    frame(isolinear::display::window& w, isolinear::layout::grid& g, const std::string& h)
        : isolinear::ui::control(g)
        , m_layout(m_grid, 2, 2, 1, 0, {3,3}, {3,2}, {0,2}, {0,3})
        , m_north(m_layout.north())
        , m_east(m_layout.east())

        , m_ne_sweep(w, m_layout.northeast(),{2,2},20,10)
        , m_se_sweep(w, m_layout.southeast(),{2,1},20,10)
        , m_s_rule(m_layout.south(), isolinear::compass::south)
        , m_n_header(w, m_north.allocate_west(w.header_font().RenderText(0xffffffff, h).size()), compass::west, h)
    {
      m_buttons.emplace_back(w, m_east.allocate_north(4), "PGUP");
      m_buttons.emplace_back(w, m_east.allocate_south(4), "PGDN");

      m_buttons.emplace_back(w, m_north.allocate_west(3), "PARENT DIR");
      m_buttons.emplace_back(w, m_north.allocate_east(2), "NEW DIR");

      m_rects.emplace_back(m_north.remainder());
      m_rects.emplace_back(m_east.remainder());

      register_child(&m_ne_sweep);
      register_child(&m_se_sweep);
      register_child(&m_s_rule);
      register_child(&m_n_header);

      for (auto& button : m_buttons) register_child(&button);
      for (auto& rect : m_rects) register_child(&rect);
    }

    isolinear::ui::header_basic& header() {
      return m_n_header;
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

  std::string path = std::getenv("DEV_DIR");

  frame myframe(window, gridfactory.root(), path);
  myframe.header().label(path);
  window.add(&myframe);

  isolinear::layout::vertical_row vrow(myframe.centre());

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

    buttons.emplace_back(window, hrow.allocate_west(1), "ICO");

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

    vrules.emplace_back(hrow.allocate_east(1), compass::centre);

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
