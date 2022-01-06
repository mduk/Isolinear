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
using isolinear::CompassLayout;
using isolinear::ui::NorthWestSweep;
using isolinear::ui::SouthWestSweep;


namespace display = isolinear::display;


namespace isompd {

  class view : public View {
    protected:
      mpdxx::client& mpdc;
      display::window& window;

    public:
      view(std::string t, Grid g, display::window& w, mpdxx::client& _mpdc)
        : View(t, g)
        , window{w}
        , mpdc{_mpdc}
      {};
  };

}

namespace isompd::now_playing {

  class view : public isompd::view {
    protected:
      bool hide = false;
      isolinear::ui::header_pair_bar title;
      isolinear::ui::header_pair_bar album;
      isolinear::ui::header_pair_bar artist;
      isolinear::ui::header_pair_bar duration;
      isolinear::ui::progress::horizontal_bar progress;

    public:
      view(Grid g, display::window& w, mpdxx::client& _mpdc)
        : isompd::view("NOW PLAYING", g, w, _mpdc)
        , title(g.Rows(3,4), w, "TITLE", "[title]")
        , album(g.Rows(5,6), w, "ALBUM", "[album]")
        , artist(g.Rows(7,8), w, "ARTIST", "[artist]")
        , duration(g.Rows(9,10), w, "DURATION", "[duration]")
        , progress(g.Rows(11,12))
      {
        RegisterChild(&title);
        RegisterChild(&album);
        RegisterChild(&artist);
        RegisterChild(&duration);
        RegisterChild(&progress);

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

      void Draw(SDL_Renderer* renderer) const override {
        if (hide) {
          return;
        }
        isompd::view::Draw(renderer);
      }
  };

}



template <class DataT, class ViewT>
class paginated_rows : public drawable {

  protected:
    Grid grid;
    display::window& window;
    int view_page = 0;
    int page_rows = 10;
    std::vector<DataT> data_rows{};
    std::vector<ViewT> view_rows{};

  public:
    paginated_rows(Grid g, display::window& w, int pr)
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
      children.clear();

      int end_index = page_rows * view_page;
      int start_index = end_index - page_rows;

      auto nrows = data_rows.size();

      for (int i = 1; i <= page_rows; i++) {
        int data_row_index = start_index + (i-1);

        if (nrows <= data_row_index) {
          continue;
        }

        view_rows.emplace_back(
            grid.Rows((i*2)-1, i*2),
            window,
            data_rows.at(data_row_index)
          );

        ViewT& row = view_rows.back();
        RegisterChild(&row);
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

    Region2D Bounds() const override { return grid.bounds; }

    void Draw(SDL_Renderer* renderer) const override {
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
            grid.Rows((i*2)-1, i*2),
            window,
            data_rows.at(data_row_index)
          );
        row.Colours(Colours());
        row.Draw(renderer);
      }
    }
};


namespace isompd::player {

  class queuerow : public isolinear::ui::label {
    protected:
      mpdxx::song song;

    public:
      queuerow(Grid g, display::window& w, mpdxx::song s)
        : isolinear::ui::label(w, g, s.Header())
        , song(s)
      {
      }
  };

  class view : public isompd::view {
    protected:
      isolinear::ui::header_basic hdrQueue;
      paginated_rows<mpdxx::song, isompd::player::queuerow> queue_pager;

      Grid gc;

      isolinear::ui::button btnPlay;
      isolinear::ui::button btnPause;
      isolinear::ui::button btnStop;

      isolinear::ui::horizontal_rule hrule1;

      isolinear::ui::button btnPrevious;
      isolinear::ui::button btnNext;

      isolinear::ui::horizontal_rule hrule2;

      isolinear::ui::button btnConsume;
      isolinear::ui::button btnRandom;
      isolinear::ui::button btnSingle;
      isolinear::ui::button btnRepeat;

      int queue_length = 0;

    public:
      view(Grid g, display::window& w, mpdxx::client& mpdc)
        : isompd::view("PLAYER", g, w,  mpdc)

        , hdrQueue   (g.Columns( 1,  6).Rows( 1,  2), w, isolinear::compass::west, "QUEUE")
        , queue_pager(g.Columns( 1,  6).Rows( 3, 10), w, 20)

        , gc(g.Columns(16,21))

        , btnPlay(    w, gc.Rows( 1, 4).Columns(1,4), "PLAY")
        , btnPause(   w, gc.Rows( 1, 4).Columns(5,6), "PAUSE")
        , btnStop(    w, gc.Rows( 5, 7).Columns(1,6), "STOP")

        , hrule1(     w, gc.Rows( 8, 8))

        , btnPrevious(w, gc.Rows( 9,10).Columns(1,3), "PREVIOUS")
        , btnNext(    w, gc.Rows( 9,10).Columns(4,6), "NEXT")

        , hrule2(     w, gc.Rows(11,11))

        , btnRepeat(  w, gc.Rows(12,13).Columns(1,4), "REPEAT")
        , btnSingle(  w, gc.Rows(12,13).Columns(5,6), "SINGLE")
        , btnConsume( w, gc.Rows(14,15).Columns(1,2), "CONSUME")
        , btnRandom(  w, gc.Rows(14,15).Columns(3,6), "RANDOM")
      {
        RegisterChild(&hdrQueue);
        RegisterChild(&queue_pager);
        RegisterChild(&btnPlay);
        RegisterChild(&btnPause);
        RegisterChild(&btnStop);
        RegisterChild(&btnPrevious);
        RegisterChild(&btnNext);
        RegisterChild(&btnConsume);
        RegisterChild(&btnRandom);
        RegisterChild(&btnSingle);
        RegisterChild(&btnRepeat);
        RegisterChild(&hrule1);
        RegisterChild(&hrule2);

        miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> queue){
          queue_pager.clear();
          for (auto& song : queue) {
            queue_pager.add_row(song);
          }
          queue_pager.page(1);
        });

        miso::connect(mpdc.signal_status, [this](mpdxx::status status){
          cout << fmt::format("PlayerView signal_status begin\n");
          btnConsume.Active(status.Consume());
          btnRandom.Active(status.Random());
          btnSingle.Active(status.Single());
          btnRepeat.Active(status.Repeat());

          if (status.IsPlaying()) {
            cout << " => Playing\n";

            btnPlay.Enable();
            btnPlay.Activate();

            btnPause.Enable();
            btnPause.Deactivate();

            btnStop.Enable();
            btnStop.Deactivate();
          }

          if (status.IsPaused()) {
            cout << " => Paused\n";

            btnPlay.Enable();
            btnPlay.Enable();
            btnPlay.Activate();

            btnPause.Enable();
            btnPause.Activate();

            btnStop.Enable();
            btnStop.Deactivate();
          }

          if (status.IsStopped()) {
            cout << " => Stopped\n";

            btnPlay.Enable();
            btnStop.Enable();
            btnStop.Activate();

            btnPause.Disable();

            btnPlay.Deactivate();
            if (queue_length > 0) {
              btnPlay.Disable();
            } else {
              btnPlay.Enable();
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

  class row : public isolinear::ui::header_east_bar {
    protected:
      mpdxx::song song;
      isolinear::ui::button& playbtn;

    public:
      row(Grid g, display::window& w, mpdxx::song s)
        : isolinear::ui::header_east_bar(g, w, s.Header())
        , song(s)
        , playbtn(AddButton("PLAY"))
      {
        miso::connect(playbtn.signal_press, [this](){
          cout << fmt::format("Play {}\n", song.Title());
        });
      }
  };

  class view : public isompd::view {
    protected:
      paginated_rows<mpdxx::song, isompd::queue::row> queue_pager;
      isolinear::ui::header_east_bar queue_pager_buttons;
      isolinear::ui::button& next_page_button;
      isolinear::ui::button& previous_page_button;

    public:
      view(Grid g, display::window& w, mpdxx::client& mpdc)
        : isompd::view("QUEUE", g, w,  mpdc)
        , queue_pager(g, w, 10)
        , queue_pager_buttons(g.Rows(21, 22), w, "##")
        , previous_page_button(queue_pager_buttons.AddButton("PREVIOUS"))
        , next_page_button(queue_pager_buttons.AddButton("NEXT"))
      {
        RegisterChild(&queue_pager);

        miso::connect(previous_page_button.signal_press, [this](){
            if (next_page_button.Disabled()) {
              next_page_button.Enable();
            }

            queue_pager.previous_page();

            if (queue_pager.on_first_page()) { previous_page_button.Disable(); }
            if (queue_pager.on_final_page()) { next_page_button.Disable(); }

            queue_pager_buttons.Label(fmt::format("Page {} of {}",
                queue_pager.current_page(), queue_pager.page_count()));
          });

        miso::connect(next_page_button.signal_press, [this](){
            if (previous_page_button.Disabled()) {
              previous_page_button.Enable();
            }

            queue_pager.next_page();

            if (queue_pager.on_first_page()) { previous_page_button.Disable(); }
            if (queue_pager.on_final_page()) { next_page_button.Disable(); }

            queue_pager_buttons.Label(fmt::format("Page {} of {}",
                queue_pager.current_page(), queue_pager.page_count()));
          });

        queue_pager_buttons.Label(fmt::format("Page {} of {}", queue_pager.current_page(), queue_pager.page_count()));

        RegisterChild(&queue_pager_buttons);

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

  class artist_row : public isolinear::ui::header_basic {
    public:
      artist_row(Grid g, display::window& w, mpdxx::artist e)
        : isolinear::ui::header_basic(g, w, isolinear::compass::west, e.Header())
      {}
  };

  class view : public isompd::view {
    protected:
      Grid artist_grid;
      Grid album_grid;

      paginated_rows<mpdxx::artist, isompd::browse::artist_row> artist_pager;

      horizontal_button_bar artist_pager_buttons;

    public:
      view(Grid g, display::window& w, mpdxx::client& _mpdc)
        : isompd::view("BROWSE", g, w, _mpdc)
        , artist_grid(g)
        , artist_pager(artist_grid, w, 10)
        , artist_pager_buttons(w, g.Rows(21, 22))
      {
        RegisterChild(&artist_pager);

        miso::connect(artist_pager_buttons.AddButton("PREVIOUS").signal_press, [this](){
          artist_pager.previous_page();
        });
        miso::connect(artist_pager_buttons.AddButton("NEXT").signal_press, [this](){
          artist_pager.next_page();
        });
        RegisterChild(&artist_pager_buttons);

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

  using isolinear::ui::vertical_button_bar;
  using isolinear::ui::horizontal_button_bar;

  class frame : public drawable {
    public:
      const std::string V_NOWPLAYING = "NOW PLAYING";
      const std::string V_QUEUE = "QUEUE";
      const std::string V_BROWSE = "BROWSE";
      const std::string V_PLAYER = "PLAYER";

    public:
      miso::signal<std::string, std::string> signal_view_change;

    protected:
      mpdxx::client& mpdc;

      CompassLayout layout;

      isolinear::ui::header_east_bar hdrFrame;
      vertical_button_bar barView;
      PlayerControlBar playerControlBar;
      NorthWestSweep sweepNorthWest;
      SouthWestSweep sweepSouthWest;

      std::map<const std::string, View*> views;
      std::string activeView = V_QUEUE;

      isompd::browse::view viewBrowse;
      isompd::now_playing::view viewNowPlaying;
      isompd::queue::view viewQueue;
      isompd::player::view viewPlayer;

    public:
      frame(Grid g, display::window& w, mpdxx::client& _mpdc)
          : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
          , hdrFrame{layout.North(), w, isolinear::compass::east, "MPD CONTROL"}
          , barView{w, layout.West()}
          , playerControlBar{w, layout.South(), mpdc}
          , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
          , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

          , mpdc(_mpdc)

          , viewNowPlaying(layout.Centre(), w, mpdc)
          , viewQueue     (layout.Centre(), w, mpdc)
          , viewBrowse    (layout.Centre(), w, mpdc)
          , viewPlayer    (layout.Centre(), w, mpdc)
      {
        RegisterChild(&hdrFrame);
        RegisterChild(&barView);
        RegisterChild(&playerControlBar);
        RegisterChild(&sweepNorthWest);
        RegisterChild(&sweepSouthWest);

        RegisterView(&viewNowPlaying);
        RegisterView(&viewQueue);
        RegisterView(&viewBrowse);
        RegisterView(&viewPlayer);

        auto switch_view = [this]() {
          auto button = miso::sender<isolinear::ui::button>();
          SwitchView(button->Label());
        };

        for (auto const& [view_name, view_ptr] : views) {
          isolinear::ui::button& view_btn = barView.AddButton(view_name);
          miso::connect(view_btn.signal_press, switch_view);
        }

        barView.GetButton(activeView).Activate();
        emit signal_view_change("", activeView);

        playerControlBar.Update();
        Update();
      }

      virtual void SwitchView(std::string view) {
        auto previousView = activeView;
        activeView = view;

        barView.DeactivateAll();
        barView.ActivateOne(activeView);

        hdrFrame.Label(fmt::format("MPD : {}", activeView));

        emit signal_view_change(previousView, activeView);
      }

      virtual Region2D Bounds() const override {
        return layout.Bounds();
      }

      virtual void OnPointerEvent(PointerEvent event) {
        drawable::OnPointerEvent(event);
        views.at(activeView)->OnPointerEvent(event);
      }

      virtual void Draw(SDL_Renderer* renderer) const {
        drawable::Draw(renderer);
        views.at(activeView)->Draw(renderer);
      }

      virtual void Colours(ColourScheme cs) {
        drawable::Colours(cs);
        for (auto const& [view_name, view_ptr] : views) {
          view_ptr->Colours(cs);
        }
      }

      void RegisterView(View* view) {
        const std::string view_name = view->Name();
        views.insert(std::pair<const std::string, View*>(view_name, view));
      }
  };

}
