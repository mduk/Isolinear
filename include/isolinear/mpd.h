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


class MPDView : public View {
  protected:
    mpdxx::client& mpdc;
    Window& window;

  public:
    MPDView(std::string t, Grid g, Window& w, mpdxx::client& _mpdc)
      : View(t, g)
      , window{w}
      , mpdc{_mpdc}
    {};
};


class NowPlayingView : public MPDView {
  protected:
    bool hide = false;
    PairHeaderBar title;
    PairHeaderBar album;
    PairHeaderBar artist;
    PairHeaderBar duration;
    HorizontalProgressBar progress;

  public:
    NowPlayingView(Grid g, Window& w, mpdxx::client& _mpdc)
      : MPDView("NOW PLAYING", g, w, _mpdc)
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
      MPDView::Draw(renderer);
    }

};



template <class T>
class paginated_rows {

  protected:
    Grid grid;
    Window& window;
    uint8_t page = 1;
    int page_rows;
    std::vector<T> rows{};

  public:
    paginated_rows(Grid g, Window& w, int pr)
      : grid(g), window(w), page_rows(pr)
    {

    }

    void add_row(T row) {
      rows.push_back(row);
    }

    void next_page() {
      page++;
    }

    void previous_page() {
      if (page == 1) {
        return;
      }
      page--;
    }

    void draw_page(SDL_Renderer* renderer, ColourScheme colours) const {
      auto nrows = rows.size();

      if (nrows == 0) {
        return;
      }

      int end_index = page_rows * page;
      int start_index = end_index - page_rows;

      for (int i = 1; i <= page_rows; i++) {
        int row_index = start_index + (i-1);

        if (nrows <= row_index) {
          continue;
        }

        EastHeaderBar songbar(
            grid.Rows((i*2)-1, i*2),
            window,
            rows.at(row_index).Header()
          );
        songbar.Colours(colours);
        songbar.Draw(renderer);
      }


    }
};


class QueueView : public MPDView {
  protected:
    paginated_rows<mpdxx::song> pager;
    HorizontalButtonBar pagerbuttons;

  public:
    QueueView(Grid g, Window& w, mpdxx::client& mpdc)
      : MPDView("QUEUE", g, w,  mpdc)
      , pager(g, w, 10)
      , pagerbuttons(w, g.Rows(21, 22))
    {
      miso::connect(pagerbuttons.AddButton("PREVIOUS").signal_press, [this](){ pager.previous_page(); });
      miso::connect(pagerbuttons.AddButton("NEXT")    .signal_press, [this](){ pager.next_page();     });
      RegisterChild(&pagerbuttons);

      miso::connect(mpdc.signal_queue, [this](std::list<mpdxx::song> queue){
        for (auto& song : queue) {
          pager.add_row(song);
        }
      });
    }

    void Draw(SDL_Renderer* renderer) const override {
      MPDView::Draw(renderer);
      pager.draw_page(renderer, Colours());
    }
};


class ArtistsView : public MPDView {
  protected:
    paginated_rows<mpdxx::artist> pager;
    HorizontalButtonBar pagerbuttons;

  public:
    ArtistsView(Grid g, Window& w, mpdxx::client& _mpdc)
      : MPDView("ARTISTS", g, w, _mpdc)
      , pager(g, w, 10)
      , pagerbuttons(w, g.Rows(21, 22))
    {
      miso::connect(pagerbuttons.AddButton("PREVIOUS").signal_press, [this](){ pager.previous_page(); });
      miso::connect(pagerbuttons.AddButton("NEXT")    .signal_press, [this](){ pager.next_page();     });
      RegisterChild(&pagerbuttons);

      miso::connect(mpdc.signal_artist_list, [this](std::list<mpdxx::artist> artist_list){
        for (auto& artist : artist_list) {
          pager.add_row(artist);
        }
      });
    }

    void Draw(SDL_Renderer* renderer) const override {
      MPDView::Draw(renderer);
      pager.draw_page(renderer, Colours());
    }
};


class MpdFrame : public Drawable {
  public:
    const std::string V_NOWPLAYING = "NOW PLAYING";
    const std::string V_QUEUE = "QUEUE";
    const std::string V_ARTISTS = "ARTISTS";

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

    ArtistsView viewArtists;
    NowPlayingView viewNowPlaying;
    QueueView viewQueue;

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
        , viewArtists   (layout.Centre(), w, mpdc)
    {
      RegisterChild(&hdrFrame);
      RegisterChild(&barView);
      RegisterChild(&playerControlBar);
      RegisterChild(&sweepNorthWest);
      RegisterChild(&sweepSouthWest);

      RegisterView(&viewNowPlaying);
      RegisterView(&viewQueue);
      RegisterView(&viewArtists);

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

