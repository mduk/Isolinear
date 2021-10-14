#include "window.h"

Window::Window(int w, int h)
  : size{w, h},
    grid{
      Region{size},
      RowHeight{100},
      NumCols{12},
      Margin{10,10},
      Gutter{10,10}
    }
{
  sdl_window = SDL_CreateWindow(
    title.c_str(),
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    size.x, size.y,
    SDL_WINDOW_OPENGL //| SDL_WINDOW_FULLSCREEN_DESKTOP
  );

  if (!sdl_window) {
    throw std::runtime_error(
      "Failed to create SDL window"
    );
  }

  sdl_renderer = SDL_CreateRenderer(
    sdl_window, -1, SDL_RENDERER_SOFTWARE
  );

  if (!sdl_renderer) {
    throw std::runtime_error(
      "Failed to create SDL renderer"
    );
  }


  SDL_SetRenderDrawBlendMode(
    sdl_renderer, SDL_BLENDMODE_BLEND
  );
  SDL_SetRenderDrawColor(sdl_renderer, 0, 0, 0, 255);
  SDL_RenderPresent(sdl_renderer);

  SDL_GetWindowSize(
    sdl_window,
    &size.x,
    &size.y
  );

  Grid grid{
    Region{size},
    RowHeight{font.Height()},
    NumCols{12},
    Margin{10,10},
    Gutter{10,10}
  };
}

void Window::Add(Drawable* drawable) {
  drawables.push_back(drawable);
}

void Window::Draw() {
  for (auto* drawable : drawables) {
    drawable->Draw(sdl_renderer);
  }
  SDL_RenderPresent(sdl_renderer);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Window::OnMouseButtonDown\n");
  Position cursor{event};

  for (auto* drawable : drawables) {
    if (drawable->Bounds().Encloses(cursor)) {
      drawable->OnMouseButtonDown(event);
      return;
    }
  }
}


void Window::draw(SDL_Renderer* r, Position p, uint32_t c) {
  filledCircleColor(r, p.x, p.y, 10, c);
}

void Window::draw(SDL_Renderer* r, Region re, uint32_t c) {
  boxColor(r, re.NearX(), re.NearY(),
               re.FarX(),  re.FarY(), c);
}

