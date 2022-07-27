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
      mpdxx::client& mpdc;
      display::window& window;

    public:
      view(std::string t, layout::grid g, display::window& w, mpdxx::client& _mpdc)
        : isolinear::view(t, g)
        , window{w}
        , mpdc{_mpdc}
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
      view(layout::grid g, display::window& w, mpdxx::client& _mpdc)
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

        miso::connect(mpdc.signal_status, [this](mpdxx::status status){
          hide = status.IsStopped();
        });

        miso::connect(mpdc.signal_current_song, [this](mpdxx::song song){
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
    layout::grid grid;
    display::window& window;
    int view_page = 0;
    int page_rows = 10;
    std::vector<DataT> data_rows{};
    std::vector<ViewT> view_rows{};

  public:
    paginated_rows(layout::grid g, display::window& w, int pr)
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


namespace isompd::player {
  namespace ui = isolinear::ui;

  class queuerow : public ui::label {
    protected:
      mpdxx::song song;

    public:
      queuerow(layout::grid g, display::window& w, mpdxx::song s)
        : ui::label(w, g, s.Header())
        , song(s)
      {
      }
  };

  class view : public isompd::view {
    protected:
      ui::header_basic hdrQueue;
      paginated_rows<mpdxx::song, isompd::player::queuerow> queue_pager;

      layout::grid gc;

      ui::button btnPlay;
      ui::button btnPause;
      ui::button btnStop;

      ui::horizontal_rule hrule1;

      ui::button btnPrevious;
      ui::button btnNext;

      ui::horizontal_rule hrule2;

      ui::button btnConsume;
      ui::button btnRandom;
      ui::button btnSingle;
      ui::button btnRepeat;

      int queue_length = 0;

    public:
      view(layout::grid g, display::window& w, mpdxx::client& mpdc)
        : isompd::view("PLAYER", g, w,  mpdc)

        , hdrQueue   (g.columns( 1,  6).rows( 1,  2), w, isolinear::compass::west, "QUEUE")
        , queue_pager(g.columns( 1,  6).rows( 3, 10), w, 20)

        , gc(g.right_columns(6))

        , btnPlay(    w, gc.rows( 1, 4).columns(1,4), "PLAY")
        , btnPause(   w, gc.rows( 1, 4).columns(5,6), "PAUSE")
        , btnStop(    w, gc.rows( 5, 7).columns(1,6), "STOP")

        , hrule1(        gc.rows( 8, 8))

        , btnPrevious(w, gc.rows( 9,10).columns(1,3), "PREVIOUS")
        , btnNext(    w, gc.rows( 9,10).columns(4,6), "NEXT")

        , hrule2(        gc.rows(11,11))

        , btnRepeat(  w, gc.rows(12,13).columns(1,4), "REPEAT")
        , btnSingle(  w, gc.rows(12,13).columns(5,6), "SINGLE")
        , btnConsume( w, gc.rows(14,15).columns(1,2), "CONSUME")
        , btnRandom(  w, gc.rows(14,15).columns(3,6), "RANDOM")
      {
        register_child(&hdrQueue);
        register_child(&queue_pager);
        register_child(&btnPlay);
        register_child(&btnPause);
        register_child(&btnStop);
        register_child(&btnPrevious);
        register_child(&btnNext);
        register_child(&btnConsume);
        register_child(&btnRandom);
        register_child(&btnSingle);
        register_child(&btnRepeat);
        register_child(&hrule1);
        register_child(&hrule2);

        miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> queue){
          queue_pager.clear();
          for (auto& song : queue) {
            queue_pager.add_row(song);
          }
          queue_pager.page(1);
        });

        miso::connect(mpdc.signal_status, [this](mpdxx::status status){
          cout << fmt::format("PlayerView signal_status begin\n");
          btnConsume.active(status.Consume());
          btnRandom.active(status.Random());
          btnSingle.active(status.Single());
          btnRepeat.active(status.Repeat());

          if (status.IsPlaying()) {
            cout << " => Playing\n";

            btnPlay.enable();
            btnPlay.activate();

            btnPause.enable();
            btnPause.deactivate();

            btnStop.enable();
            btnStop.deactivate();
          }

          if (status.IsPaused()) {
            cout << " => Paused\n";

            btnPlay.enable();
            btnPlay.enable();
            btnPlay.activate();

            btnPause.enable();
            btnPause.activate();

            btnStop.enable();
            btnStop.deactivate();
          }

          if (status.IsStopped()) {
            cout << " => Stopped\n";

            btnPlay.enable();
            btnStop.enable();
            btnStop.activate();

            btnPause.disable();

            btnPlay.deactivate();
            if (queue_length > 0) {
              btnPlay.disable();
            } else {
              btnPlay.enable();
            }
          }

          cout << fmt::format("PlayerView signal_status end\n");
        });

        miso::connect(    btnPlay.signal_press, [&]() { mpdc.Play();          });
        miso::connect(    btnStop.signal_press, [&]() { mpdc.Stop();          });
        miso::connect(btnPrevious.signal_press, [&]() { mpdc.Previous();      });
        miso::connect(    btnNext.signal_press, [&]() { mpdc.Next();          });
        miso::connect(   btnPause.signal_press, [&]() { mpdc.TogglePause();   });
        miso::connect( btnConsume.signal_press, [&]() { mpdc.ToggleConsume(); });
        miso::connect(  btnRandom.signal_press, [&]() { mpdc.ToggleRandom();  });
        miso::connect(  btnRandom.signal_press, [&]() { mpdc.ToggleSingle();  });
        miso::connect(  btnRandom.signal_press, [&]() { mpdc.ToggleRepeat();  });
      }
  };

}


namespace isompd::queue {
  namespace ui = isolinear::ui;

  class row : public ui::header_east_bar {
    protected:
      mpdxx::song song;
      ui::button& playbtn;

    public:
      row(layout::grid g, display::window& w, mpdxx::song s)
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
      view(layout::grid g, display::window& w, mpdxx::client& mpdc)
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
      artist_row(layout::grid g, display::window& w, mpdxx::artist e)
        : ui::header_basic(g, w, isolinear::compass::west, e.Header())
      {}
  };

  class view : public isompd::view {
    protected:
      layout::grid artist_grid;
      layout::grid album_grid;

      paginated_rows<mpdxx::artist, isompd::browse::artist_row> artist_pager;

      ui::horizontal_button_bar artist_pager_buttons;

    public:
      view(layout::grid g, display::window& w, mpdxx::client& _mpdc)
        : isompd::view("BROWSE", g, w, _mpdc)
        , artist_grid(g)
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

        miso::connect(mpdc.signal_artist_list, [this](std::list<mpdxx::artist> artist_list){
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
      const std::string V_PLAYER = "PLAYER";

    public:
      miso::signal<std::string, std::string> signal_view_change;

    protected:
      mpdxx::client& mpdc;

      isolinear::layout::compass layout;

      ui::header_east_bar hdrFrame;
      ui::vertical_button_bar barView;
      PlayerControlBar playerControlBar;
      ui::north_west_sweep sweepNorthWest;
      ui::south_west_sweep sweepSouthWest;

      std::map<const std::string, isolinear::view*> views;
      std::string activeView = V_QUEUE;

      isompd::browse::view viewBrowse;
      isompd::now_playing::view viewNowPlaying;
      isompd::queue::view viewQueue;
      isompd::player::view viewPlayer;

    public:
      frame(layout::grid g, display::window& w, mpdxx::client& _mpdc)
          : layout{ g, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
          , hdrFrame{layout.north(), w, isolinear::compass::east, "MPD CONTROL"}
          , barView{w, layout.west()}
          , playerControlBar{w, layout.south(), mpdc}
          , sweepNorthWest{w, layout.northwest(), {3, 2}, 100, 50}
          , sweepSouthWest{w, layout.southwest(), {3, 2}, 100, 50}

          , mpdc(_mpdc)

          , viewNowPlaying(layout.centre(), w, mpdc)
          , viewQueue     (layout.centre(), w, mpdc)
          , viewBrowse    (layout.centre(), w, mpdc)
          , viewPlayer    (layout.centre(), w, mpdc)
      {
        register_child(&hdrFrame);
        register_child(&barView);
        register_child(&playerControlBar);
        register_child(&sweepNorthWest);
        register_child(&sweepSouthWest);

        RegisterView(&viewNowPlaying);
        RegisterView(&viewQueue);
        RegisterView(&viewBrowse);
        RegisterView(&viewPlayer);

        auto switch_view = [this]() {
          auto button = miso::sender<ui::button>();
          SwitchView(button->label());
        };

        for (auto const& [view_name, view_ptr] : views) {
          ui::button& view_btn = barView.add_button(view_name);
          miso::connect(view_btn.signal_press, switch_view);
        }

          barView.get_button(activeView).activate();
        emit signal_view_change("", activeView);
      }

      virtual void SwitchView(std::string view) {
        auto previousView = activeView;
        activeView = view;

          barView.deactivate_all();
        barView.activate_one(activeView);

        hdrFrame.label(fmt::format("MPD : {}", activeView));

        emit signal_view_change(previousView, activeView);
      }

      virtual isolinear::geometry::region bounds() const override {
        return layout.bounds();
      }

      virtual void on_pointer_event(isolinear::event::pointer event) {
        control::on_pointer_event(event);
        views.at(activeView)->on_pointer_event(event);
      }

      virtual void draw(SDL_Renderer* renderer) const {
        control::draw(renderer);
        views.at(activeView)->draw(renderer);
      }

      virtual void colours(isolinear::theme::colour_scheme cs) {
        control::colours(cs);
        for (auto const& [view_name, view_ptr] : views) {
          view_ptr->colours(cs);
        }
      }

      void RegisterView(isolinear::view* view) {
        const std::string view_name = view->Name();
        views.insert(std::pair<const std::string, isolinear::view*>(view_name, view));
      }
  };

}
