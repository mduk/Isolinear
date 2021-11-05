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

bool drawdebug = false;

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

  int outer_radius{90};
  int inner_radius{50};

  int north_frame = 2,
       east_frame = 3,
      south_frame = 2,
       west_frame = 4;

  Vector2D nesize{4,3};
  Vector2D sesize{5,4};
  Vector2D swsize{6,5};
  Vector2D nwsize{7,6};

  VerticalButtonBar eastbar{window, framegrid.SubGrid(
      framegrid.MaxColumns() - (east_frame-1),
      nesize.y + 1,

      framegrid.MaxColumns(),
      framegrid.MaxRows() - sesize.y
    )};
  eastbar.AddButton("92-5668");
  window.Add(&eastbar);

  VerticalButtonBar westbar{window, framegrid.SubGrid(
      1,
      nwsize.y + 1,

      west_frame,
      framegrid.MaxRows() - swsize.y
    )};
  westbar.AddButton("92-5668");
  window.Add(&westbar);

  HorizontalButtonBar northbar{window, framegrid.SubGrid(
      nwsize.x + 1, 1,
      framegrid.MaxColumns() - nesize.x, north_frame
    )};
  northbar.AddButton("92-5668");
  window.Add(&northbar);

  HorizontalButtonBar southbar{window, framegrid.SubGrid(
      swsize.x + 1, framegrid.MaxRows() - (south_frame-1),
      framegrid.MaxColumns() - sesize.x, framegrid.MaxRows()
    )};
  southbar.AddButton("516.375");
  window.Add(&southbar);



  NorthEastSweep northeastsweep{window,
      framegrid.SubGrid(
          framegrid.MaxColumns() - (nesize.x - 1),
          1,

          framegrid.MaxColumns(),
          nesize.y
        ),
      Vector2D{ east_frame, north_frame },
      outer_radius,
      inner_radius
    };
  window.Add(&northeastsweep);

  SouthEastSweep southeastsweep{window,
      framegrid.SubGrid(
          framegrid.MaxColumns() - (sesize.x - 1),
          framegrid.MaxRows() - (sesize.y - 1),

          framegrid.MaxColumns(),
          framegrid.MaxRows()
        ),
      Vector2D{ east_frame, south_frame },
      outer_radius,
      inner_radius
    };
  window.Add(&southeastsweep);

  SouthWestSweep southwestsweep{window,
      framegrid.SubGrid(
          1,
          framegrid.MaxRows() - (swsize.y - 1),

          swsize.x,
          framegrid.MaxRows()
        ),
      Vector2D{ west_frame, south_frame },
      outer_radius,
      inner_radius
    };
  window.Add(&southwestsweep);

  NorthWestSweep northwestsweep{window,
      framegrid.SubGrid(
          1,
          1,

          nwsize.x,
          nwsize.y
        ),
      Vector2D{ west_frame, north_frame },
      outer_radius,
      inner_radius
    };
  window.Add(&northwestsweep);


  window.Colours(blue_alert_colours);

  // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // //

  bool running = true;

  printf("LOOP\n");
  while (running) {

    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 255);
    SDL_RenderClear(window.sdl_renderer);
    SDL_SetRenderDrawColor(window.sdl_renderer, 0, 0, 0, 0);

    if (drawdebug) {
      window.grid.Draw(window.sdl_renderer);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
      switch (e.type) {

        case SDL_KEYDOWN: {
          switch (e.key.keysym.sym) {
            case SDLK_ESCAPE: running = false; break;
            case         'c': SDL_ShowCursor(!SDL_ShowCursor(SDL_QUERY)); break;
            case         'd': window.Colours(debug_colours); break;
            case         'r': window.Colours(red_alert_colours); break;
            case         'y': window.Colours(yellow_alert_colours); break;
            case         'b': window.Colours(blue_alert_colours); break;
            case         'g': drawdebug = !drawdebug; break;
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
