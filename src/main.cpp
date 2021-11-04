#include <stdio.h>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include "geometry.h"
#include "window.h"
#include "grid.h"
#include "shapes.h"
#include "elbo.h"
#include "header.h"
#include "buttonbar.h"
#include "sweep.h"
#include "pointerevent.h"

using namespace std;

string http_req(string url) {
  list<string> headers;
  headers.push_back("Accept: application/json");

  curlpp::Cleanup myCleanup;
  ostringstream response_stream;
  curlpp::Easy req;
  req.setOpt<curlpp::options::Url>(url);
  req.setOpt<curlpp::options::HttpHeader>(headers);
  req.setOpt<curlpp::options::WriteStream>(&response_stream);
  req.perform();

  return string(response_stream.str());
}

int main(int argc, char* argv[]) {
  printf("INIT\n");

  srand(time(NULL));

  //curlpp::initialize();
  SDL_Init(SDL_INIT_VIDEO);
  TTF_Init();
  SDL_ShowCursor(SDL_DISABLE);

  int number_of_displays = SDL_GetNumVideoDisplays();
  std::vector<SDL_Rect> displays;

  for (int i=0; i<number_of_displays; i++) {
    SDL_Rect bounds{};
    SDL_GetDisplayBounds(i, &bounds);
    displays.push_back(bounds);

    printf("%d: %d,%d +(%d,%d) [%d]\n",
        i,
        bounds.w,
        bounds.h,
        bounds.x,
        bounds.y,
        bounds.w * bounds.h
      );
  }

  SDL_Rect display = displays.back();

  Window window(
      Position2D{display},
      Size2D{display}
    );

  Header header{window.grid.Rows(1,2), window, " HEADER TITLE "};
  header.AddButton("01-8854");
  header.AddButton("02-5861");
  header.AddButton("03-8854");
  window.Add(&header);

  Grid framegrid = window.grid.Rows(4, window.grid.MaxRows());

  Vector2D sweepsize{4,3};
  Vector2D ports{3,1};
       int outer_radius{90};
       int inner_radius{50};



  NorthEastSweep northeastsweep{window,
      framegrid.SubGrid(
          framegrid.MaxColumns() - sweepsize.x - 1, 1,
          framegrid.MaxColumns(), sweepsize.y
        ),
      sweepsize, Vector2D{2,1}, outer_radius, inner_radius
    };
  window.Add(&northeastsweep);

  VerticalButtonBar eastbar{window, framegrid.SubGrid(
      framegrid.MaxColumns() - northeastsweep.VerticalPortSize() - 1, sweepsize.y + 1,
      framegrid.MaxColumns(), framegrid.MaxRows() - sweepsize.y - 1
    )};
  window.Add(&eastbar);

  SouthEastSweep southeastsweep{window,
      framegrid.SubGrid(
          framegrid.MaxColumns() - sweepsize.x - 1, framegrid.MaxRows() - sweepsize.y - 1,
          framegrid.MaxColumns(), framegrid.MaxRows()
        ),
      sweepsize, Vector2D{2,2}, outer_radius, inner_radius
    };
  window.Add(&southeastsweep);

  SouthWestSweep southwestsweep{window,
      framegrid.SubGrid(
          1, framegrid.MaxRows() - sweepsize.y - 1,
          sweepsize.x, framegrid.MaxRows()
        ),
      sweepsize, Vector2D{3,2}, outer_radius, inner_radius
    };
  window.Add(&southwestsweep);

  NorthWestSweep northwestsweep{window,
      framegrid.SubGrid(
          1, 1,
          sweepsize.x, sweepsize.y
        ),
      sweepsize, Vector2D{3,1}, outer_radius, inner_radius
    };
  window.Add(&northwestsweep);


/*

       int max_cols = window.grid.MaxColumns();
       int max_rows = 8;

       int sweep_offset = 3;

  window.Add(&northeastsweep);
  window.Add(&southeastsweep);
  window.Add(&southwestsweep);
  window.Add(&northwestsweep);

  Button rightbutton{window, window.grid.CalculateGridRegion(
      max_cols - sweepsize.x + 3, sweep_offset + sweepsize.y + 1,
      max_cols                  , sweep_offset + sweepsize.y + 2
    ), window.Colours(), "65-8858"};
  window.Add(&rightbutton);


  Region2D topbarregion = window.grid.CalculateGridRegion(
      sweepsize.x + 1, sweep_offset + 1,
      max_cols - sweepsize.x, sweep_offset + 1
    );
  Quad topbar(topbarregion);
  window.Add(&topbar);

  Grid bottombargrid = window.grid.SubGrid(
      sweepsize.x + 1,        sweep_offset + max_rows - 1,
      max_cols - sweepsize.x, sweep_offset + max_rows
    );
  HorizontalButtonBar bottombar(bottombargrid, window);
  bottombar.AddButton("04-2993");
  bottombar.AddButton("07-9825");
  window.Add(&bottombar);

  Grid mygrid = window.grid.Rows(sweep_offset + max_rows + 3,
                                 sweep_offset + max_rows + 20);
  VerticalButtonBar verticalbar(mygrid.Columns(1,2), window);
  verticalbar.AddButton("12-3456");
  verticalbar.AddButton("12-7891");
  window.Add(&verticalbar);
*/

  window.Colours(blue_alert_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;
  bool drawgrid = false;

  printf("LOOP\n");
  while (running) {

    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawgrid) {
      window.grid.Draw(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: running = false; break;
            case         'c': SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY)); break;
            case         'r': window.Colours(red_alert_colours); break;
            case         'y': window.Colours(yellow_alert_colours); break;
            case         'b': window.Colours(blue_alert_colours); break;
            case         'g': drawgrid = !drawgrid; break;
          }
          break;
        }
/*
        case SDL_MOUSEMOTION: {
          int x = e.motion.x,
              y = e.motion.y;

          Position2D pos{x, y};
          int gx = window.grid.PositionColumnIndex(pos),
              gy = window.grid.PositionRowIndex(pos);

          std::stringstream ss;
          ss << "Mouse X=" << x << " Y=" << y << " Grid Col=" << gx << " Row=" << gy;

          window.Title(ss.str());
          break;
        }
*/
        case SDL_FINGERDOWN:      window.OnPointerEvent(PointerEvent{e.tfinger, window.size}); break;
        case SDL_MOUSEBUTTONDOWN: window.OnPointerEvent(PointerEvent{e.button }); break;

        case SDL_WINDOWEVENT: {
          switch (e.window.event) {
            case SDL_WINDOWEVENT_RESIZED: {
              window.OnWindowResize(e.window);
              break;
            };
          }
          break;
        }

        case SDL_QUIT: {
          running = false;
          break;
        }
      }
    }

    window.Draw();

    SDL_RenderPresent(window.sdl_renderer);
  }

  return 0;
}
