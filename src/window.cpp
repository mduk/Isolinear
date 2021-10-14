#include "window.h"

Window::Window(int w, int h)
  : size{w, h},
    font{
      "/home/daniel/.fonts/Swiss 911 Ultra Compressed BT.ttf",
      64
    },
    grid{
      Region{
        Position{0,0},
        size
      },
      RowHeight{100},
      NumCols{12},
      Margin{10,10},
      Gutter{10,10}
    }
{
  this->sdl_window = SDL_CreateWindow(
    "Isolinear",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    this->size.x, this->size.y,
    SDL_WINDOW_OPENGL //| SDL_WINDOW_FULLSCREEN_DESKTOP
  );

  if (!this->sdl_window) {
    throw std::runtime_error(
      "Failed to create SDL window"
    );
  }

  this->sdl_renderer = SDL_CreateRenderer(
    this->sdl_window, -1, SDL_RENDERER_SOFTWARE
  );

  if (!this->sdl_renderer) {
    throw std::runtime_error(
      "Failed to create SDL renderer"
    );
  }


  SDL_SetRenderDrawBlendMode(
    this->sdl_renderer, SDL_BLENDMODE_BLEND
  );
  SDL_SetRenderDrawColor(this->sdl_renderer, 0, 0, 0, 255);
  SDL_RenderPresent(this->sdl_renderer);

  SDL_GetWindowSize(
    this->sdl_window,
    &this->size.x,
    &this->size.y
  );

  Grid grid{
    Region{Position{0,0}, size},
    RowHeight{font.Height()},
    NumCols{12},
    Margin{10,10},
    Gutter{10,10}
  };
}

void Window::Add(Drawable* drawable) {
  this->drawables.push_back(drawable);
}

void Window::Draw() {
  for (auto* drawable : this->drawables) {
    drawable->Draw(this->sdl_renderer);
  }
  SDL_RenderPresent(this->sdl_renderer);
}

void Window::OnMouseButtonDown(SDL_MouseButtonEvent& event) {
  printf("Window::OnMouseButtonDown\n");
  Position cursor{event};

  for (auto* drawable : this->drawables) {
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

