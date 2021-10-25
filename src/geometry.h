#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "drawable.h"
#include "colours.h"


class Vector2D {
  public:
    int x, y;

    Vector2D()
        : x{0}, y{0}
      {};

    Vector2D(int _x, int _y)
        : x{_x}, y{_y}
      {};

    Vector2D(SDL_MouseButtonEvent e)
        : x{e.x}, y{e.y}
      {};

    Vector2D(SDL_MouseMotionEvent e)
        : x{e.x}, y{e.y}
      {};

    Vector2D(SDL_Surface* s)
        : x{s->w}, y{s->h}
      {};

    Vector2D Add(Vector2D c) const {
      return Vector2D{
          x + c.x,
          y + c.y
        };
    }

    Vector2D Subtract(Vector2D c) const {
      return Vector2D{
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
    int NorthEastX() const { return x;     }
    int NorthEastY() const { return 0;     }
    int SouthEastX() const { return x;     }
    int SouthEastY() const { return y;     }
    int SouthWestX() const { return 0;     }
    int SouthWestY() const { return y;     }
    int NorthWestX() const { return 0;     }
    int NorthWestY() const { return 0;     }

    Vector2D Centre()    const { return Vector2D{ CentreX(),    CentreY()    }; }
    Vector2D North()     const { return Vector2D{ NorthX(),     NorthY()     }; }
    Vector2D East()      const { return Vector2D{ EastX(),      EastY()      }; }
    Vector2D South()     const { return Vector2D{ SouthX(),     SouthY()     }; }
    Vector2D West()      const { return Vector2D{ WestX(),      WestY()      }; }
    Vector2D NorthEast() const { return Vector2D{ NorthEastX(), NorthEastY() }; }
    Vector2D SouthEast() const { return Vector2D{ SouthEastX(), SouthEastY() }; }
    Vector2D SouthWest() const { return Vector2D{ SouthWestX(), SouthWestY() }; }
    Vector2D NorthWest() const { return Vector2D{ NorthWestX(), NorthWestY() }; }
};


class Position2D : public Vector2D {
  public:
    Position2D() : Vector2D() {}
    Position2D(Vector2D c) : Vector2D{c} {}
    Position2D(int x, int y) : Vector2D(x, y) {};
    Position2D(SDL_MouseButtonEvent e) : Vector2D(e) {};
};

class Size2D : public Vector2D {
  public:
    Size2D() : Vector2D() {};
    Size2D(int w) : Vector2D{w, w} {};
    Size2D(int w, int h) : Vector2D(w, h) {};
    Size2D(SDL_Surface* s) : Vector2D(s) {};
};


class Region2D : public Drawable {
  protected:
    Position2D _position;
    Size2D _size;

  public:
    Region2D()
        : _position{0,0}, _size{0,0}
      {};

    Region2D(Size2D s)
        : _position{0,0}, _size{s}
      {};

    Region2D(SDL_Rect r)
        : _position{r.x, r.y}, _size{r.w, r.h}
      {};

    Region2D(Position2D _p, Size2D _s)
        : _position{_p}, _size{_s}
      {};

    Region2D(Position2D n, Position2D f)
        : _position{n}, _size{ f.x - n.x,
                             f.y - n.y }
      {};

    Region2D(int _x, int _y, int _w, int _h)
        : _position{_x, _y}, _size{_w, _h}
      {};


    void Resize(Size2D newsize) {
      _size = newsize;
    }

    virtual void Print() const {
      printf("Region<0x%08X> %d,%d (%d,%d) %d,%d\n",
          this,
          _position.x, _position.y,
          _size.x, _size.y,
          _position.x+_size.x, _position.y+_size.y
        );
    }


    // Sources of truth
    virtual Position2D Origin() const { return _position; };
    virtual Size2D     Size()   const { return _size; };


    // X, Y, W, H shortcuts
    int X() const { return Origin().x; }
    int Y() const { return Origin().y; }
    int W() const { return Size().x; }
    int H() const { return Size().y; }


    // Near and Far Point Positions
    Position2D Near() const { return Origin(); }
          int NearX() const { return Origin().x; }
          int NearY() const { return Origin().y; }

    Position2D Far() const { return Origin().Add(Size()); }
          int FarX() const { return Far().x; }
          int FarY() const { return Far().y; }

    // Compass points
    Position2D Centre()     const { return Origin().Add(Size().Centre());    }
    Position2D North()      const { return Origin().Add(Size().North());     }
    Position2D East()       const { return Origin().Add(Size().East());      }
    Position2D South()      const { return Origin().Add(Size().South());     }
    Position2D West()       const { return Origin().Add(Size().West());      }
    Position2D SouthWest()  const { return Origin().Add(Size().SouthWest()); }
    Position2D NorthWest()  const { return Origin().Add(Size().NorthWest()); }
    Position2D SouthEast()  const { return Origin().Add(Size().SouthEast()); }
    Position2D NorthEast()  const { return Origin().Add(Size().NorthEast()); }

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
    Region2D AlignCentre(Size2D s)    const { return Region2D{    Centre().Subtract(s.Centre()   ), s }; }
    Region2D AlignNorth(Size2D s)     const { return Region2D{     North().Subtract(s.North()    ), s }; }
    Region2D AlignEast(Size2D s)      const { return Region2D{      East().Subtract(s.East()     ), s }; }
    Region2D AlignSouth(Size2D s)     const { return Region2D{     South().Subtract(s.South()    ), s }; }
    Region2D AlignWest(Size2D s)      const { return Region2D{      West().Subtract(s.West()     ), s }; }
    Region2D AlignNorthEast(Size2D s) const { return Region2D{ NorthEast().Subtract(s.NorthEast()), s }; }
    Region2D AlignSouthEast(Size2D s) const { return Region2D{ SouthEast().Subtract(s.SouthEast()), s }; }
    Region2D AlignSouthWest(Size2D s) const { return Region2D{ SouthWest().Subtract(s.SouthWest()), s }; }
    Region2D AlignNorthWest(Size2D s) const { return Region2D{ NorthWest().Subtract(s.NorthWest()), s }; }

    // Compass Quadrants
    Region2D NorthEastQuadrant() const { return Region2D{ North(),       East() }; }
    Region2D SouthEastQuadrant() const { return Region2D{ Centre(), SouthEast() }; }
    Region2D SouthWestQuadrant() const { return Region2D{ West(),       South() }; }
    Region2D NorthWestQuadrant() const { return Region2D{ NorthWest(), Centre() }; }

    // Halfs
    Region2D TopHalf()    const { return Region2D{ NorthWest(), East()      }; }
    Region2D BottomHalf() const { return Region2D{ West(),      SouthEast() }; }
    Region2D LeftHalf()   const { return Region2D{ NorthWest(), South()     }; }
    Region2D RightHalf()  const { return Region2D{ North(),     SouthEast() }; }

    bool Encloses(Vector2D point) const {
      return ( NearX() <= point.x )
          && ( NearY() <= point.y )
          && ( point.x <= FarX()  )
          && ( point.y <= FarY()  );
    }

    bool Encloses(Region2D r) const {
      return Encloses(r.Near())
          && Encloses(r.Far());
    }



    SDL_Rect SdlRect() const {
      return SDL_Rect{
          _position.x,
          _position.y,
          Size().x,
          Size().y
        };
    }

    void OnMouseButtonDown(SDL_MouseButtonEvent& e) {
      printf("Region: %d,%d (%d,%d) %d,%d \n",
          NearX(), NearY(),
          Size().x,  Size().y,
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
      filledCircleColor(renderer,    CentreX(),    CentreY(), 6, 0xaa000000);
      filledCircleColor(renderer,     NorthX(),     NorthY(), 4, 0xaaff0000);
      filledCircleColor(renderer, NorthEastX(), NorthEastY(), 4, 0xaa00ffff);
      filledCircleColor(renderer,      EastX(),      EastY(), 4, 0xaaff0000);
      filledCircleColor(renderer, SouthEastX(), SouthEastY(), 4, 0xaaffff00);
      filledCircleColor(renderer,     SouthX(),     SouthY(), 4, 0xaa00ff00);
      filledCircleColor(renderer, SouthWestX(), SouthWestY(), 4, 0xaa00ffff);
      filledCircleColor(renderer,      WestX(),      WestY(), 4, 0xaa00ff00);
      filledCircleColor(renderer, NorthWestX(), NorthWestY(), 4, 0xaaffff00);
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
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
      boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
      boxColor(renderer, NearX()+1, NearY()+1, FarX()-2, FarY()-2, 0xff000000);
    }

    void ArcNorthWest(SDL_Renderer* renderer, Colour colour) const {
      filledPieColor(renderer,
        FarX(), FarY(),
        Size().x,
        180, 270,
        colour
      );
    }

};
