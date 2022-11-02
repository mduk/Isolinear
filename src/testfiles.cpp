#include "init.h"
#include "ui.h"
#include <filesystem>

namespace fs = std::filesystem;

class frame_vbar : public isolinear::ui::control {

protected:
    isolinear::display::window& m_window;
    isolinear::layout::vertical_row m_layout;
    std::list<isolinear::ui::button> m_buttons;

public:
    frame_vbar(isolinear::display::window& w, isolinear::layout::grid g)
        : isolinear::ui::control(g)
        , m_window(w)
        , m_layout(g)
    {}

public:
    isolinear::layout::vertical_row& layout() {
      return m_layout;
    }

    isolinear::ui::button& add_button_north(std::string label, uint width) {
      auto& button = m_buttons.emplace_back(m_window, m_layout.allocate_north(width), label);
      register_child(&button);
      return button;
    }

    isolinear::ui::button& add_button_south(std::string label, uint width) {
      auto& button = m_buttons.emplace_back(m_window, m_layout.allocate_south(width), label);
      register_child(&button);
      return button;
    }

    void draw(SDL_Renderer *renderer) const override {
      control::draw(renderer);
      m_layout.remainder().bounds().fill(renderer, colours().frame);
    }
};

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
    isolinear::display::window& m_window;
    isolinear::layout::compass m_layout;

    frame_hbar m_north;
    frame_vbar m_east;
    frame_hbar m_south;
    frame_vbar m_west;

    isolinear::ui::north_west_sweep m_nw_sweep;
    isolinear::ui::south_west_sweep m_sw_sweep;

public:
    frame(isolinear::display::window& w, isolinear::layout::grid g)
        : isolinear::ui::control(g)
        , m_window(w)
        , m_layout(m_grid, 2, 2, 1, 2, {0}, {0}, {3,2}, {3,3})

        , m_north(w, m_layout.north())
        , m_east(w, m_layout.east())
        , m_south(w, m_layout.south())
        , m_west(w, m_layout.west())

        , m_nw_sweep(w, m_layout.northwest(),{2,2},20,10)
        , m_sw_sweep(w, m_layout.southwest(),{2,1},20,10)
    {
      register_child(&m_north);
      register_child(&m_east);
      register_child(&m_south);
      register_child(&m_west);

      register_child(&m_nw_sweep);
      register_child(&m_sw_sweep);
    }

    frame_hbar& north() { return m_north; }
    frame_hbar& south() { return m_south; }
};

class filer : public frame {
protected:
    fs::path m_path{"/"};
    std::list<isolinear::ui::button> m_file_rows{};
    isolinear::layout::vertical_row m_vrow;
    bool m_show_hidden{false};

public:
    filer(isolinear::display::window& w, isolinear::layout::grid g)
      : frame(w, g)
      , m_path(std::getenv("HOME"))
      , m_vrow(m_layout.centre())
      {
        update();
      }

protected:
    void update() {
      m_north.header(m_path);

      if (m_file_rows.size() > 0) {
        m_file_rows.clear();
        return;
      }

      for (const auto& entry : fs::directory_iterator(m_path)) {
        if (!m_show_hidden && entry.path().filename().string()[0] == '.') {
          continue;
        }

        if (!is_directory(entry.path())) {
          continue;
        }

        auto& row = m_file_rows.emplace_back(m_window, m_vrow.allocate_north(2), entry.path().filename());
        row.colours(colours());
        miso::connect(row.signal_press, [&, entry](){
          path(entry.path());
          std::cout << fmt::format("cd {}\n", (std::string) entry.path());
        });
      }
    }

public:
    fs::path path() {
      return m_path;
    }

    void path(fs::path path) {
      m_path = path;
      update();
    }

    void draw(SDL_Renderer *renderer) const override {
      control::draw(renderer);
    }
};

int main(int argc, char* argv[]) {
  namespace geometry = isolinear::geometry;

  auto work_guard = asio::make_work_guard(isolinear::io_context);

  isolinear::init();
  auto& window = isolinear::new_window();

  isolinear::layout::gridfactory gridfactory(window.region(), {60,30}, {6,6});

  auto& root = gridfactory.root();
  isolinear::layout::horizontal_row hrow(root);

  isolinear::ui::header_east_bar info_frame(window, hrow.allocate_east(10).rows(1,2), "FILE DETAIL");
  window.add(&info_frame);

  filer list_frame(window, hrow.remainder());
  window.add(&list_frame);

  auto& btn_updir = list_frame.north().add_button_west("PARENT DIR", 3);
  miso::connect(btn_updir.signal_press, [&list_frame](){
    list_frame.path(list_frame.path().parent_path());
  });

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