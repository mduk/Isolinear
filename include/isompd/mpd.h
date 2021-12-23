#pragma once

#include <list>
#include <map>
#include <string>
#include <iostream>

#include <fmt/core.h>

#include "miso.h"

#include "geometry.h"
#include "header.h"
#include "window.h"
#include "mpdxx.h"
#include "progressbar.h"
#include "view.h"

#include "mpd/playercontrolbar.h"


using std::cerr;

namespace isompd {

  class view : public View {
    protected:
      mpdxx::client& mpdc;
      Window& window;

    public:
      view(std::string t, Grid g, Window& w, mpdxx::client& _mpdc)
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
      PairHeaderBar title;
      PairHeaderBar album;
      PairHeaderBar artist;
      PairHeaderBar duration;
      HorizontalProgressBar progress;

    public:
      view(Grid g, Window& w, mpdxx::client& _mpdc)
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
class paginated_rows : public Drawable {

  protected:
    Grid grid;
    Window& window;
    uint8_t view_page = 1;
    int page_rows;
    std::vector<DataT> data_rows{};
    std::vector<ViewT> view_rows{};

  public:
    paginated_rows(Grid g, Window& w, int pr)
      : grid(g), window(w), page_rows(pr)
    {

    }

    void clear() {
      data_rows.clear();
    }

    void add_row(DataT row) {
      data_rows.push_back(row);
    }

    void page(int p) {
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

    void next_page() {
      page(view_page + 1);
    }

    void previous_page() {
      if (view_page == 1) {
        return;
      }
      page(view_page - 1);
    }

    Region2D Bounds() const override {
      return grid.bounds;
    }

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

namespace isompd::browse {

  class artist_row : public BasicHeader {
    public:
      artist_row(Grid g, Window& w, mpdxx::artist e)
        : BasicHeader(g, w, Compass::WEST, e.Header())
      {}
  };

  class view : public isompd::view {
    protected:
      Grid artist_grid;
      Grid album_grid;

      paginated_rows<mpdxx::artist, isompd::browse::artist_row> artist_pager;

      HorizontalButtonBar artist_pager_buttons;

    public:
      view(Grid g, Window& w, mpdxx::client& _mpdc)
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


namespace isompd::queue {

  class row : public EastHeaderBar {
    protected:
      mpdxx::song song;
      Button& playbtn;

    public:
      row(Grid g, Window& w, mpdxx::song s)
        : EastHeaderBar(g, w, s.Header())
        , song(s)
        , playbtn(AddButton("PLAY"))
      {
        miso::connect(playbtn.signal_press, [this](){
            cout << "BTN\n";
          cout << fmt::format("Play {}\n", song.Title());
        });
      }
  };

  class view : public isompd::view {
    protected:
      paginated_rows<mpdxx::song, isompd::queue::row> queue_pager;
      HorizontalButtonBar queue_pager_buttons;

    public:
      view(Grid g, Window& w, mpdxx::client& mpdc)
        : isompd::view("QUEUE", g, w,  mpdc)
        , queue_pager(g, w, 10)
        , queue_pager_buttons(w, g.Rows(21, 22))
      {
        RegisterChild(&queue_pager);

        miso::connect(queue_pager_buttons.AddButton("PREVIOUS").signal_press, [this](){ queue_pager.previous_page(); });
        miso::connect(queue_pager_buttons.AddButton("NEXT")    .signal_press, [this](){ queue_pager.next_page();     });
        RegisterChild(&queue_pager_buttons);

        miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> queue){
          queue_pager.clear();
          for (auto& song : queue) {
            queue_pager.add_row(song);
          }
        });
      }
  };


}




class MpdFrame : public Drawable {
  public:
    const std::string V_NOWPLAYING = "NOW PLAYING";
    const std::string V_QUEUE = "QUEUE";
    const std::string V_BROWSE = "BROWSE";

  public:
    miso::signal<std::string, std::string> signal_view_change;

  protected:
    mpdxx::client& mpdc;

    CompassLayout layout;

    EastHeaderBar hdrFrame;
    VerticalButtonBar barView;
    PlayerControlBar playerControlBar;
    NorthWestSweep sweepNorthWest;
    SouthWestSweep sweepSouthWest;

    std::map<const std::string, View*> views;
    std::string activeView = V_QUEUE;

    isompd::browse::view viewBrowse;
    isompd::now_playing::view viewNowPlaying;
    isompd::queue::view viewQueue;

  public:
    MpdFrame(Grid g, Window& w, mpdxx::client& _mpdc)
        : layout{ g, w, 2, 0, 2, 3, {0,0}, {0,0}, {4,3}, {4,3} }
        , hdrFrame{layout.North(), w, Compass::EAST, "MPD CONTROL"}
        , barView{w, layout.West()}
        , playerControlBar{w, layout.South(), mpdc}
        , sweepNorthWest{w, layout.NorthWest(), {3,2}, 100, 50}
        , sweepSouthWest{w, layout.SouthWest(), {3,2}, 100, 50}

        , mpdc(_mpdc)

        , viewNowPlaying(layout.Centre(), w, mpdc)
        , viewQueue     (layout.Centre(), w, mpdc)
        , viewBrowse   (layout.Centre(), w, mpdc)
    {
      RegisterChild(&hdrFrame);
      RegisterChild(&barView);
      RegisterChild(&playerControlBar);
      RegisterChild(&sweepNorthWest);
      RegisterChild(&sweepSouthWest);

      RegisterView(&viewNowPlaying);
      RegisterView(&viewQueue);
      RegisterView(&viewBrowse);

      auto switch_view = [this]() {
        auto button = miso::sender<Button>();
        SwitchView(button->Label());
      };

      for (auto const& [view_name, view_ptr] : views) {
        Button& view_btn = barView.AddButton(view_name);
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
      Drawable::OnPointerEvent(event);
      views.at(activeView)->OnPointerEvent(event);
    }

    virtual void Draw(SDL_Renderer* renderer) const {
      Drawable::Draw(renderer);
      views.at(activeView)->Draw(renderer);
    }

    virtual void Colours(ColourScheme cs) {
      Drawable::Colours(cs);
      for (auto const& [view_name, view_ptr] : views) {
        view_ptr->Colours(cs);
      }
    }

    void RegisterView(View* view) {
      const std::string view_name = view->Name();
      views.insert(std::pair<const std::string, View*>(view_name, view));
    }
};

