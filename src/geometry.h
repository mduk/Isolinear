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
    Size(int w) : Coordinate{w, w} {};
    Size(int w, int h) : Coordinate(w, h) {};
    Size(SDL_Surface* s) : Coordinate(s) {};
};


class Region : public Drawable {
  protected:
    Position _position;
    Size _size;

  public:
    Region()
        : _position{0,0}, _size{0,0}
      {};

    Region(Size s)
        : _position{0,0}, _size{s}
      {};

    Region(SDL_Rect r)
        : _position{r.x, r.y}, _size{r.w, r.h}
      {};

    Region(Position _p, Size _s)
        : _position{_p}, _size{_s}
      {};

    Region(Position n, Position f)
        : _position{n}, _size{ f.x - n.x,
                             f.y - n.y }
      {};

    Region(int _x, int _y, int _w, int _h)
        : _position{_x, _y}, _size{_w, _h}
      {};


    void Resize(Size newsize) {
      _size = newsize;
    }



    // Sources of truth
    virtual Position Origin()  const { return _position; };
    virtual Size     GetSize() const { return _size; };


    // X, Y, W, H shortcuts
    int X() const { return Origin().x; }
    int Y() const { return Origin().y; }
    int W() const { return GetSize().x; }
    int H() const { return GetSize().y; }

    // Near and Far Point Positions
    Position Near()  const { return Origin(); }
         int NearX() const { return Origin().x; }
         int NearY() const { return Origin().y; }

    Position Far()  const { return Origin().Add(GetSize()); }
         int FarX() const { return Far().x; }
         int FarY() const { return Far().y; }

    // Compass points
    Position Centre()     const { return Origin().Add(GetSize().Centre());    }
    Position North()      const { return Origin().Add(GetSize().North());     }
    Position East()       const { return Origin().Add(GetSize().East());      }
    Position South()      const { return Origin().Add(GetSize().South());     }
    Position West()       const { return Origin().Add(GetSize().West());      }
    Position SouthWest()  const { return Origin().Add(GetSize().SouthWest()); }
    Position NorthWest()  const { return Origin().Add(GetSize().NorthWest()); }
    Position SouthEast()  const { return Origin().Add(GetSize().SouthEast()); }
    Position NorthEast()  const { return Origin().Add(GetSize().NorthEast()); }

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
          _position.x,
          _position.y,
          GetSize().x,
          GetSize().y
        };
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& e) {
      printf("Region: %d,%d (%d,%d) %d,%d \n",
          NearX(), NearY(),
          GetSize().x,  GetSize().y,
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
      filledCircleColor(renderer,    CentreX(),    CentreY(), 4, 0xaa00ff00);
      filledCircleColor(renderer,     NorthX(),     NorthY(), 4, 0xaaff0000);
      filledCircleColor(renderer, NorthEastX(), NorthEastY(), 4, 0xaa0000ff);
      filledCircleColor(renderer,      EastX(),      EastY(), 4, 0xaaff0000);
      filledCircleColor(renderer, SouthEastX(), SouthEastY(), 4, 0xaa0000ff);
      filledCircleColor(renderer,     SouthX(),     SouthY(), 4, 0xaaff0000);
      filledCircleColor(renderer, SouthWestX(), SouthWestY(), 4, 0xaa0000ff);
      filledCircleColor(renderer,      WestX(),      WestY(), 4, 0xaaff0000);
      filledCircleColor(renderer, NorthWestX(), NorthWestY(), 4, 0xaa0000ff);
      lineColor(renderer,
          NorthWestX(), NorthWestY(),
          SouthEastX(), SouthEastY(),
          0xaa000000
        );
      lineColor(renderer,
          NorthEastX(), NorthEastY(),
          SouthWestX(), SouthWestY(),
          0xaa000000
        );
    }


    void Fill(SDL_Renderer* renderer, Colour colour) const {
      boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
    }

    void Stroke(SDL_Renderer* renderer, Colour colour) const {
      boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
      boxColor(renderer, NearX()+1, NearY()+1, FarX()-2, FarY()-2, 0xff000000);
    }

    void ArcNorthWest(SDL_Renderer* renderer, Colour colour) const {
      filledPieColor(renderer,
        FarX(), FarY(),
        GetSize().x,
        180, 270,
        colour
      );
    }

};
