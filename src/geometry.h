#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>



class Coordinate {
  public:
    int x, y;

    Coordinate()
        : x{0}, y{0}
      {};

    Coordinate(int _x, int _y)
        : x{_x}, y{_y}
      {};

    Coordinate(SDL_MouseButtonEvent e)
        : x{e.x}, y{e.y}
      {};

    Coordinate(SDL_MouseMotionEvent e)
        : x{e.x}, y{e.y}
      {};

    Coordinate(SDL_Surface* s)
        : x{s->w}, y{s->h}
      {};

    Coordinate Add(Coordinate c) const {
      return Coordinate{
          x + c.x,
          y + c.y
        };
    }

    void Subtract(Coordinate c) {
      this->x -= c.x;
      this->y -= c.y;
    }

    int CentreX()    const { return x / 2; }
    int CentreY()    const { return y / 2; }
    int NorthX()     const { return x / 2; }
    int NorthY()     const { return 0;     }
    int EastX()      const { return x;     }
    int EastY()      const { return y / 2; }
    int SouthX()     const { return x / 2; }
    int SouthY()     const { return y;     }
    int WestX()      const { return 0;     }
    int WestY()      const { return y / 2; }
    int NorthEastX() const { return 0;     }
    int NorthEastY() const { return y;     }
    int SouthEastX() const { return x;     }
    int SouthEastY() const { return y;     }
    int SouthWestX() const { return 0;     }
    int SouthWestY() const { return y;     }
    int NorthWestX() const { return 0;     }
    int NorthWestY() const { return 0;     }

    Coordinate Centre()    const { return Coordinate{ CentreX(),    CentreY()    }; }
    Coordinate North()     const { return Coordinate{ NorthX(),     NorthY()     }; }
    Coordinate East()      const { return Coordinate{ EastX(),      EastY()      }; }
    Coordinate South()     const { return Coordinate{ SouthX(),     SouthY()     }; }
    Coordinate West()      const { return Coordinate{ WestX(),      WestY()      }; }
    Coordinate NorthEast() const { return Coordinate{ NorthEastX(), NorthEastY() }; }
    Coordinate SouthEast() const { return Coordinate{ SouthEastX(), SouthEastY() }; }
    Coordinate SouthWest() const { return Coordinate{ SouthWestX(), SouthWestY() }; }
    Coordinate NorthWest() const { return Coordinate{ NorthWestX(), NorthWestY() }; }
};


class Position : public Coordinate {
  public:
    Position() : Coordinate() {}
    Position(Coordinate c) : Coordinate{c} {}
    Position(int x, int y) : Coordinate(x, y) {};
    Position(SDL_MouseButtonEvent e) : Coordinate(e) {};
};

class Size : public Coordinate {
  public:
    Size() : Coordinate() {};
    Size(int w, int h) : Coordinate(w, h) {};
    Size(SDL_Surface* s) : Coordinate(s) {};
};


class Region {
  public:
    Position position;
    Size size;

    Region()
        : position{0,0}, size{0,0}
      {};

    Region(Position _p, Size _s)
        : position{_p}, size{_s}
      {};

    Region(Position n, Position f)
        : position{n}, size{ f.x - n.x,
                             f.y - n.y }
      {};

    Region(int _x, int _y, int _w, int _h)
        : position{_x, _y}, size{_w, _h}
      {};

    // Near and Far Point Positions

    Position Near()  const { return this->position; }
         int NearX() const { return this->position.x; }
         int NearY() const { return this->position.y; }

    Position Far()  const { return Position{ FarX(), FarY() }; }
         int FarX() const { return this->position.x + this->size.x; }
         int FarY() const { return this->position.y + this->size.y; }

    Position Centre()     const { return position.Add(size.Centre());    }
    Position North()      const { return position.Add(size.North());     }
    Position East()       const { return position.Add(size.East());      }
    Position South()      const { return position.Add(size.South());     }
    Position West()       const { return position.Add(size.West());      }
    Position SouthWest()  const { return position.Add(size.SouthWest()); }
    Position NorthWest()  const { return position.Add(size.NorthWest()); }
    Position SouthEast()  const { return position.Add(size.SouthEast()); }
    Position NorthEast()  const { return position.Add(size.NorthEast()); }

    int CentreX()    const { return Centre().x;    }
    int CentreY()    const { return Centre().y;    }
    int NorthX()     const { return North().x;     }
    int NorthY()     const { return North().y;     }
    int EastX()      const { return East().x;      }
    int EastY()      const { return East().y;      }
    int SouthX()     const { return South().x;     }
    int SouthY()     const { return South().y;     }
    int WestX()      const { return West().x;      }
    int WestY()      const { return West().y;      }
    int NorthEastX() const { return NorthEast().x; }
    int NorthEastY() const { return NorthEast().y; }
    int SouthEastX() const { return SouthEast().x; }
    int SouthEastY() const { return SouthEast().y; }
    int SouthWestX() const { return SouthWest().x; }
    int SouthWestY() const { return SouthWest().y; }
    int NorthWestX() const { return NorthWest().x; }
    int NorthWestY() const { return NorthWest().y; }

    // Compass Point Regions

    Region AlignCentre(Size& s) const {
      return Region{
          CentreX() - (s.x / 2),
          CentreY() - (s.y / 2),
          s.x, s.y
        };
    }

    Region AlignNorth(Size& s) const {
      return Region{
          NorthX() - (s.x / 2),
          NorthY(),
          s.x, s.y
        };
    }

    Region AlignSouth(Size& s) const {
      return Region{};
    }

    Region AlignEast(Size& s) const {
      return Region{
          EastX() - s.x,
          EastY() - (s.y / 2),
          s.x, s.y
        };
    }

    Region AlignSouthEast(Size& s) const {
      return Region{
          SouthEastX() - s.x,
          SouthEastY() - s.y,
          s.x, s.y
        };
    }

    // Compass Quadrants
    Region NorthEastQuadrant() const {
      return Region{ North(), East() };
    }

    Region SouthEastQuadrant() const {
      return Region{ Centre(), SouthEast() };
    }

    Region SouthWestQuadrant() const {
      return Region{ West(), South() };
    }

    Region NorthWestQuadrant() const {
      return Region{ NorthWest(), Centre() };
    }


    bool Encloses(Coordinate& point) const {
      return ( NearX() <= point.x )
          && ( NearY() <= point.y )
          && ( point.x <= FarX()  )
          && ( point.y <= FarY()  );
    };



    SDL_Rect AsSdlRect() const {
      return SDL_Rect{
          position.x,
          position.y,
          size.x,
          size.y
        };
    }



    void RenderText(
        SDL_Renderer* renderer,
        std::string text
    ) const {
      TTF_Font* font = TTF_OpenFont(
          "/home/daniel/.fonts/Swiss 911 Ultra Compressed BT.ttf",
          64
        );

      if (!font) {
        fprintf(stderr, "Couldn't load font: %s\n", TTF_GetError());
        throw std::runtime_error(
          "Failed to load font"
        );
      }

      SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            font,
            text.c_str(),
            SDL_Color{255,255,255}
          );

      TTF_CloseFont(font);

      SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer, surface
          );

      Size label_size{ surface };
      Region label_region = AlignSouth(label_size);
      label_region.position.Subtract(Coordinate{5,0});

      SDL_Rect label_rect = label_region.AsSdlRect();
      SDL_RenderFillRect(renderer, &label_rect);

      SDL_RenderCopy(
          renderer,
          texture,
          NULL,
          &label_rect
        );

      SDL_FreeSurface(surface);
      SDL_DestroyTexture(texture);
    }
};
