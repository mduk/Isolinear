#include "init.h"
#include "ui.h"
#include <filesystem>


class frame : public isolinear::ui::control {

protected:
    isolinear::layout::compass m_layout;
    isolinear::ui::north_east_sweep m_ne_sweep;
    isolinear::ui::south_east_sweep m_se_sweep;
    isolinear::ui::horizontal_rule m_n_rule;
    isolinear::ui::horizontal_rule m_s_rule;
    isolinear::ui::header_east_bar m_n_header;
    isolinear::ui::button m_button_ok;
    isolinear::ui::button m_button_cancel;

public:
    frame(isolinear::display::window& w, isolinear::layout::grid& g)
        : isolinear::ui::control(g)
        , m_layout(m_grid, 2, 2, 1, 0, {3,3}, {3,2}, {0,2}, {0,2})
        , m_ne_sweep(w, m_layout.northeast(),{2,2},20,10)
        , m_se_sweep(w, m_layout.southeast(),{2,1},20,10)
        , m_n_rule(m_layout.north(), isolinear::compass::north)
        , m_s_rule(m_layout.south(), isolinear::compass::south)
        , m_button_ok(w, m_layout.east().top_rows(4), "UP")
        , m_button_cancel(w, m_layout.east().bottom_rows(4), "DOWN")
        , m_n_header(w, m_layout.north(), "")
    {
      register_child(&m_ne_sweep);
      register_child(&m_se_sweep);
      register_child(&m_n_rule);
      register_child(&m_s_rule);
      register_child(&m_n_header);
      register_child(&m_button_ok);
      register_child(&m_button_cancel);
    }

    isolinear::ui::header_east_bar& header() {
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

  std::string path = std::getenv("HOME");

  frame myframe(window, gridfactory.root());
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

    buttons.emplace_back(window, hrow.allocate_left(2), "ICO");

    auto& others_execute = buttons.emplace_back(window,hrow.allocate_right(1),"X");
    others_execute.active((perms & fs::perms::others_exec) != fs::perms::none);
    auto& others_write = buttons.emplace_back(window,hrow.allocate_right(1),"W");
    others_write.active((perms & fs::perms::others_write) != fs::perms::none);
    auto& others_read = buttons.emplace_back(window,hrow.allocate_right(1),"R");
    others_read.active((perms & fs::perms::others_read) != fs::perms::none);

    vrules.emplace_back(hrow.allocate_right(1), compass::centre);

    auto& owner_execute = buttons.emplace_back(window,hrow.allocate_right(1),"X");
    owner_execute.active((perms & fs::perms::owner_exec) != fs::perms::none);
    auto& owner_write = buttons.emplace_back(window,hrow.allocate_right(1),"W");
    owner_write.active((perms & fs::perms::owner_write) != fs::perms::none);
    auto& owner_read = buttons.emplace_back(window,hrow.allocate_right(1),"R");
    owner_read.active((perms & fs::perms::owner_read) != fs::perms::none);

    vrules.emplace_back(hrow.allocate_right(1), compass::centre);

    auto& group_execute = buttons.emplace_back(window,hrow.allocate_right(1),"X");
    group_execute.active((perms & fs::perms::group_exec) != fs::perms::none);
    auto& group_write = buttons.emplace_back(window,hrow.allocate_right(1),"W");
    group_write.active((perms & fs::perms::group_write) != fs::perms::none);
    auto& group_read = buttons.emplace_back(window,hrow.allocate_right(1),"R");
    group_read.active((perms & fs::perms::group_read) != fs::perms::none);

    vrules.emplace_back(hrow.allocate_right(1), compass::centre);

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
