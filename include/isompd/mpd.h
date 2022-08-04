#pragma once

#include <list>
#include <map>
#include <string>
#include <iostream>

#include <fmt/core.h>

#include "miso.h"

#include "geometry.h"
#include "display.h"
#include "mpdxx.h"
#include "view.h"
#include "ui.h"


#include "mpd/playercontrolbar.h"


using std::cerr;


namespace display = isolinear::display;


namespace isompd {

  class view : public isolinear::view {
    protected:
      mpdxx::client& m_mpdc;
      display::window& m_window;

    public:
      view(std::string t, isolinear::layout::grid g, display::window& w, mpdxx::client& _mpdc)
        : isolinear::view(t, g)
        , m_window{w}
        , m_mpdc{_mpdc}
      {};
  };

}

namespace isompd::now_playing {
  namespace ui = isolinear::ui;

  class view : public isompd::view {
    protected:
      bool hide = false;
      ui::header_pair_bar title;
      ui::header_pair_bar album;
      ui::header_pair_bar artist;
      ui::header_pair_bar duration;
      ui::horizontal_progress_bar progress;

    public:
      view(isolinear::layout::grid g, display::window& w, mpdxx::client& _mpdc)
        : isompd::view("NOW PLAYING", g, w, _mpdc)
        , title(g.rows(3,4), w, "TITLE", "[title]")
        , album(g.rows(5,6), w, "ALBUM", "[album]")
        , artist(g.rows(7,8), w, "ARTIST", "[artist]")
        , duration(g.rows(9,10), w, "DURATION", "[duration]")
        , progress(g.rows(11,12))
      {
        register_child(&title);
        register_child(&album);
        register_child(&artist);
        register_child(&duration);
        register_child(&progress);

        miso::connect(m_mpdc.signal_status, [this](mpdxx::status status){
          hide = status.IsStopped();
        });

        miso::connect(m_mpdc.signal_current_song, [this](mpdxx::song song){
          title.Right(song.Title());
          album.Right(song.Album());
          artist.Right(song.Artist());
          duration.Right(song.DurationString());
        });
      }

      void draw(SDL_Renderer* renderer) const override {
        if (hide) {
          return;
        }
        isompd::view::draw(renderer);
      }
  };

}



template <class DataT, class ViewT>
class paginated_rows : public isolinear::ui::control {

  protected:
    isolinear::layout::grid grid;
    display::window& window;
    int view_page = 0;
    int page_rows = 10;
    std::vector<DataT> data_rows{};
    std::vector<ViewT> view_rows{};

  public:
    paginated_rows(isolinear::layout::grid g, display::window& w, int pr)
      : grid(g), window(w), page_rows(pr)
    {

    }

    void clear() {
      data_rows.clear();
      view_page = 0;
    }

    void add_row(DataT row) {
      data_rows.push_back(row);
    }

    int page_count() const {
      int nrows = data_rows.size();
      int npages = nrows / page_rows;
      if (nrows % page_rows > 0) {
        npages++;
      }
      return npages;
    }

    void page(int p) {
      int previous_page = view_page;

      view_page = p;

      view_rows.clear();
      m_children.clear();

      int end_index = page_rows * view_page;
      int start_index = end_index - page_rows;

      auto nrows = data_rows.size();

      for (int i = 1; i <= page_rows; i++) {
        int data_row_index = start_index + (i-1);

        if (nrows <= data_row_index) {
          continue;
        }

        view_rows.emplace_back(
            grid.rows((i*2)-1, i*2),
            window,
            data_rows.at(data_row_index)
          );

        ViewT& row = view_rows.back();
        register_child(&row);
      }
    }

    int current_page() const { return view_page; }
    bool on_first_page() const { return view_page == 1; }
    bool on_final_page() const { return view_page == page_count(); }

    void next_page() {
      page(view_page + 1);
    }

    void previous_page() {
      if (view_page == 1) {
        return;
      }
      page(view_page - 1);
    }

    isolinear::geometry::region bounds() const override { return grid.bounds(); }

    void draw(SDL_Renderer* renderer) const override {
      auto nrows = data_rows.size();

      if (nrows == 0) {
        return;
      }

      int end_index = page_rows * view_page;
      int start_index = end_index - page_rows;

      for (int i = 1; i <= page_rows; i++) {
        int data_row_index = start_index + (i-1);

        if (nrows <= data_row_index) {
          continue;
        }

        ViewT row(
            grid.rows((i*2)-1, i*2),
            window,
            data_rows.at(data_row_index)
          );
        row.colours(colours());
        row.draw(renderer);
      }
    }
};


namespace isompd::queue {
  namespace ui = isolinear::ui;

  class row : public ui::header_east_bar {
    protected:
      mpdxx::song song;
      ui::button& playbtn;

    public:
      row(isolinear::layout::grid g, display::window& w, mpdxx::song s)
        : ui::header_east_bar(g, w, s.Header())
        , song(s)
        , playbtn(add_button("PLAY"))
      {
        miso::connect(playbtn.signal_press, [this](){
          cout << fmt::format("Play {}\n", song.Title());
        });
      }
  };

  class view : public isompd::view {
    protected:
      paginated_rows<mpdxx::song, isompd::queue::row> queue_pager;
      ui::header_east_bar queue_pager_buttons;
      ui::button& next_page_button;
      ui::button& previous_page_button;

    public:
      view(isolinear::layout::grid g, display::window& w, mpdxx::client& mpdc)
        : isompd::view("QUEUE", g, w,  mpdc)
        , queue_pager(g, w, 10)
        , queue_pager_buttons(g.bottom_rows(4).top_rows(2), w, "##")
        , previous_page_button(queue_pager_buttons.add_button("PREVIOUS"))
        , next_page_button(queue_pager_buttons.add_button("NEXT"))
      {
        register_child(&queue_pager);

        miso::connect(previous_page_button.signal_press, [this](){
            if (next_page_button.disabled()) {
              next_page_button.enable();
            }

            queue_pager.previous_page();

            if (queue_pager.on_first_page()) { previous_page_button.disable(); }
            if (queue_pager.on_final_page()) { next_page_button.disable(); }

            queue_pager_buttons.label(fmt::format("Page {} of {}",
                queue_pager.current_page(), queue_pager.page_count()));
          });

        miso::connect(next_page_button.signal_press, [this](){
            if (previous_page_button.disabled()) {
              previous_page_button.enable();
            }

            queue_pager.next_page();

            if (queue_pager.on_first_page()) { previous_page_button.disable(); }
            if (queue_pager.on_final_page()) { next_page_button.disable(); }

            queue_pager_buttons.label(fmt::format("Page {} of {}",
                queue_pager.current_page(), queue_pager.page_count()));
          });

        queue_pager_buttons.label(fmt::format("Page {} of {}", queue_pager.current_page(), queue_pager.page_count()));

        register_child(&queue_pager_buttons);

        miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> queue){
          queue_pager.clear();
          for (auto& song : queue) {
            queue_pager.add_row(song);
          }
          queue_pager.page(1);
        });
      }
  };


}




namespace isompd::browse {
  namespace ui = isolinear::ui;

  class artist_row : public ui::header_basic {
    public:
      artist_row(isolinear::layout::grid g, display::window& w, mpdxx::artist e)
        : ui::header_basic(g, w, isolinear::compass::west, e.Header())
      {}
  };

  class view : public isompd::view {
    protected:
      isolinear::layout::grid artist_grid;
      isolinear::layout::grid album_grid;

      paginated_rows<mpdxx::artist, isompd::browse::artist_row> artist_pager;

      ui::horizontal_button_bar artist_pager_buttons;

    public:
      view(isolinear::layout::grid g, display::window& w, mpdxx::client& _mpdc)
        : isompd::view("BROWSE", g, w, _mpdc)
        , artist_grid(g)
        , album_grid(g)
        , artist_pager(artist_grid, w, 10)
        , artist_pager_buttons(w, g.bottom_rows(4).top_rows(2))
      {
        register_child(&artist_pager);

        miso::connect(artist_pager_buttons.add_button("PREVIOUS").signal_press, [this](){
          artist_pager.previous_page();
        });
        miso::connect(artist_pager_buttons.add_button("NEXT").signal_press, [this](){
          artist_pager.next_page();
        });
        register_child(&artist_pager_buttons);

        miso::connect(m_mpdc.signal_artist_list, [this](std::list<mpdxx::artist> artist_list){
          artist_pager.clear();
          for (auto& artist : artist_list) {
            artist_pager.add_row(artist);
          }
        });
      }
  };
}

namespace isompd {
  namespace ui = isolinear::ui;


  class frame : public ui::control {
    public:
      const std::string V_NOWPLAYING = "NOW PLAYING";
      const std::string V_QUEUE = "QUEUE";
      const std::string V_BROWSE = "BROWSE";

    public:
      miso::signal<std::string, std::string> signal_view_change;

    protected:
      mpdxx::client& m_mpdc;

      isolinear::layout::compass m_layout;

      ui::header_east_bar m_frame_header;
      ui::vertical_button_bar m_view_buttons;
      PlayerControlBar m_player_control_bar;
      ui::north_west_sweep m_northwest_sweep;
      ui::south_west_sweep m_southwest_sweep;

      std::map<const std::string, isolinear::view*> m_views;
      std::string m_active_view = V_QUEUE;

      isompd::browse::view m_browse_view;
      isompd::now_playing::view m_now_playing_view;
      isompd::queue::view m_queue_view;

    public:
      frame(isolinear::layout::grid g, display::window& w, mpdxx::client& _mpdc)
          : m_layout{g, 2, 0, 2, 3, {0, 0}, {0, 0}, {4, 3}, {4, 3} }
          , m_frame_header{m_layout.north(), w, isolinear::compass::east, "MPD CONTROL"}
          , m_view_buttons{w, m_layout.west()}
          , m_player_control_bar{w, m_layout.south(), m_mpdc}
          , m_northwest_sweep{w, m_layout.northwest(), {3, 2}, 50, 20}
          , m_southwest_sweep{w, m_layout.southwest(), {3, 2}, 50, 20}

          , m_mpdc(_mpdc)

          , m_now_playing_view(m_layout.centre(), w, m_mpdc)
          , m_queue_view     (m_layout.centre(), w, m_mpdc)
          , m_browse_view    (m_layout.centre(), w, m_mpdc)
      {
        register_child(&m_frame_header);
        register_child(&m_view_buttons);
        register_child(&m_player_control_bar);
        register_child(&m_northwest_sweep);
        register_child(&m_southwest_sweep);

        register_view(&m_now_playing_view);
        register_view(&m_queue_view);
        register_view(&m_browse_view);

        auto change_view_handler = [this]() {
          auto button = miso::sender<ui::button>();
          switch_view(button->label());
        };

        for (auto const& [view_name, view_ptr] : m_views) {
          ui::button& view_btn = m_view_buttons.add_button(view_name);
          miso::connect(view_btn.signal_press, change_view_handler);
        }

          m_view_buttons.get_button(m_active_view).activate();
        emit signal_view_change("", m_active_view);
      }

      virtual void switch_view(std::string view) {
        auto previousView = m_active_view;
        m_active_view = view;

        m_view_buttons.deactivate_all();
        m_view_buttons.activate_one(m_active_view);

        m_frame_header.label(fmt::format("MPD : {}", m_active_view));

        emit signal_view_change(previousView, m_active_view);
      }

      isolinear::geometry::region bounds() const override {
        return m_layout.bounds();
      }

      void on_pointer_event(isolinear::event::pointer event) override {
        control::on_pointer_event(event);
        m_views.at(m_active_view)->on_pointer_event(event);
      }

      void on_keyboard_event(isolinear::event::keyboard event) override {
        switch (event.code()) {
          case SDLK_2:
            switch_view(V_QUEUE);
            break;
          case SDLK_3:
            switch_view(V_BROWSE);
            break;
          case SDLK_4:
            switch_view(V_BROWSE);
            break;
          default:
            break;
        }
      }

      virtual void draw(SDL_Renderer* renderer) const {
        control::draw(renderer);
        m_views.at(m_active_view)->draw(renderer);
      }

      virtual void colours(isolinear::theme::colour_scheme cs) {
        control::colours(cs);
        for (auto const& [view_name, view_ptr] : m_views) {
          view_ptr->colours(cs);
        }
      }

      void register_view(isolinear::view* view) {
        const std::string view_name = view->name();
        m_views.insert(std::pair<const std::string, isolinear::view*>(view_name, view));
      }
  };

}
