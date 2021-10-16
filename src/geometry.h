#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "colours.h"


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

    Coordinate Subtract(Coordinate c) const {
      return Coordinate{
          x - c.x,
          y - c.y
        };
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


class Region : public Drawable {
  public:
    Position position;
    Size size;

    Region()
        : position{0,0}, size{0,0}
      {};

    Region(Size s)
        : position{0,0}, size{s}
      {};

    Region(SDL_Rect r)
        : position{r.x, r.y}, size{r.w, r.h}
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

    // Compass Alignment
    Region AlignCentre(Size s)    const { return Region{    Centre().Subtract(s.Centre()   ), s }; }
    Region AlignNorth(Size s)     const { return Region{     North().Subtract(s.North()    ), s }; }
    Region AlignEast(Size s)      const { return Region{      East().Subtract(s.East()     ), s }; }
    Region AlignSouth(Size s)     const { return Region{     South().Subtract(s.South()    ), s }; }
    Region AlignWest(Size s)      const { return Region{      West().Subtract(s.West()     ), s }; }
    Region AlignNorthEast(Size s) const { return Region{ NorthEast().Subtract(s.NorthEast()), s }; }
    Region AlignSouthEast(Size s) const { return Region{ SouthEast().Subtract(s.SouthEast()), s }; }
    Region AlignSouthWest(Size s) const { return Region{ SouthWest().Subtract(s.SouthWest()), s }; }
    Region AlignNorthWest(Size s) const { return Region{ NorthWest().Subtract(s.NorthWest()), s }; }

    // Compass Quadrants
    Region NorthEastQuadrant() const { return Region{ North(),       East() }; }
    Region SouthEastQuadrant() const { return Region{ Centre(), SouthEast() }; }
    Region SouthWestQuadrant() const { return Region{ West(),       South() }; }
    Region NorthWestQuadrant() const { return Region{ NorthWest(), Centre() }; }

    // Halfs
    Region TopHalf()    const { return Region{ NorthWest(), East()      }; }
    Region BottomHalf() const { return Region{ West(),      SouthEast() }; }
    Region LeftHalf()   const { return Region{ NorthWest(), South()     }; }
    Region RightHalf()  const { return Region{ North(),     SouthEast() }; }

    bool Encloses(Coordinate& point) const {
      return ( NearX() <= point.x )
          && ( NearY() <= point.y )
          && ( point.x <= FarX()  )
          && ( point.y <= FarY()  );
    };



    SDL_Rect SdlRect() const {
      return SDL_Rect{
          position.x,
          position.y,
          size.x,
          size.y
        };
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& e) {
      printf("Region: %d,%d (%d,%d) %d,%d \n",
          NearX(), NearY(),
          size.x,  size.y,
          FarX(),  FarY()
        );
      printf(" Click: %d,%d (%d,%d local)\n",
          e.x, e.y,
          e.x - NearX(),
          e.y - NearY()
        );
    }

    void Draw(SDL_Renderer* renderer) const {
      Fill(renderer, 0xaaffffff);
    }


    void Fill(SDL_Renderer* renderer, Colour colour) const {
      boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
    }

    void Stroke(SDL_Renderer* renderer, Colour colour) const {
      boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
      boxColor(renderer, NearX()+1, NearY()+1, FarX()-2, FarY()-2, 0xff000000);
    }

};
