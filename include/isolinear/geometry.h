#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "colours.h"

namespace isolinear::geometry {

  enum Compass {
    CENTRE,
    NORTH, NORTHEAST, EAST, SOUTHEAST,
    SOUTH, SOUTHWEST, WEST, NORTHWEST
  };

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
      Position2D(SDL_Rect r) : Vector2D(r.x, r.y) {};

      void Draw(SDL_Renderer* renderer) {
        filledEllipseColor(renderer, x, y,  5,  5, 0xff00ffff);
      }
  };

  class Size2D : public Vector2D {
    public:
      Size2D() : Vector2D() {};
      Size2D(int w) : Vector2D{w, w} {};
      Size2D(int w, int h) : Vector2D(w, h) {};
      Size2D(SDL_Surface* s) : Vector2D(s) {};
      Size2D(SDL_Rect r) : Vector2D(r.w, r.h) {};
  };


  class Region2D {
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

      Region2D(Position2D a, Position2D b) {
          Position2D near{
              std::min(a.x, b.x),
              std::min(a.y, b.y)
            };

          Position2D far{
              std::max(a.x, b.x),
              std::max(a.y, b.y)
            };

          _position = near;
          _size = Size2D{
              far.x - near.x,
              far.y - near.y
            };
        };

      Region2D(int _x, int _y, int _w, int _h)
          : _position{_x, _y}, _size{_w, _h}
        {};


      void Resize(Size2D newsize) {
        _size = newsize;
      }

      virtual void Print() const {
        printf("Region %d,%d (%d,%d) %d,%d\n",
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
      Position2D Centre()     const { return Point(Compass::CENTRE   ); }
      Position2D North()      const { return Point(Compass::NORTH    ); }
      Position2D NorthEast()  const { return Point(Compass::NORTHEAST); }
      Position2D East()       const { return Point(Compass::EAST     ); }
      Position2D SouthEast()  const { return Point(Compass::SOUTHEAST); }
      Position2D South()      const { return Point(Compass::SOUTH    ); }
      Position2D SouthWest()  const { return Point(Compass::SOUTHWEST); }
      Position2D West()       const { return Point(Compass::WEST     ); }
      Position2D NorthWest()  const { return Point(Compass::NORTHWEST); }

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

      // Compass Points
      Position2D Point(Compass align) const {
        switch (align) {
          case    Compass::CENTRE: return Origin().Add(Size().Centre());
          case     Compass::NORTH: return Origin().Add(Size().North());
          case Compass::NORTHEAST: return Origin().Add(Size().NorthEast());
          case      Compass::EAST: return Origin().Add(Size().East());
          case Compass::SOUTHEAST: return Origin().Add(Size().SouthEast());
          case     Compass::SOUTH: return Origin().Add(Size().South());
          case Compass::SOUTHWEST: return Origin().Add(Size().SouthWest());
          case      Compass::WEST: return Origin().Add(Size().West());
          case Compass::NORTHWEST: return Origin().Add(Size().NorthWest());
        }
        return Position2D();
      }

      // Compass Alignment
      Region2D Align(Compass align, Size2D s) const {
        switch (align) {
          case    Compass::CENTRE: return Region2D{    Centre().Subtract(s.Centre()   ), s };
          case     Compass::NORTH: return Region2D{     North().Subtract(s.North()    ), s };
          case Compass::NORTHEAST: return Region2D{ NorthEast().Subtract(s.NorthEast()), s };
          case      Compass::EAST: return Region2D{      East().Subtract(s.East()     ), s };
          case Compass::SOUTHEAST: return Region2D{ SouthEast().Subtract(s.SouthEast()), s };
          case     Compass::SOUTH: return Region2D{     South().Subtract(s.South()    ), s };
          case Compass::SOUTHWEST: return Region2D{ SouthWest().Subtract(s.SouthWest()), s };
          case      Compass::WEST: return Region2D{      West().Subtract(s.West()     ), s };
          case Compass::NORTHWEST: return Region2D{ NorthWest().Subtract(s.NorthWest()), s };
        }
        return Region2D();
      }

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

      virtual void Fill(SDL_Renderer* renderer, Colour colour) const {
        boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
      }

      virtual void RoundedFill(SDL_Renderer* renderer, int radius, Colour colour) const {
        roundedBoxColor(renderer, NearX(), NearY(), FarX(), FarY(), radius, colour);
      }

      virtual void Ellipse(SDL_Renderer* renderer, Colour colour) const {
        filledEllipseColor(renderer,
            CentreX(), CentreY(),
            W()/2, H()/2,
            colour
          );
      }

      virtual void Stroke(SDL_Renderer* renderer, Colour colour) const {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        boxColor(renderer, NearX()  , NearY()  , FarX()  , FarY()  , colour);
        boxColor(renderer, NearX()+1, NearY()+1, FarX()-2, FarY()-2, 0xff000000);
      }

      virtual void QuadrantArc(SDL_Renderer* renderer, Compass orientation, Colour colour) const {
        switch (orientation) {
          case Compass::NORTHEAST: filledPieColor(renderer, SouthWestX(), SouthWestY(), W(), 270,   0, colour); break;
          case Compass::SOUTHEAST: filledPieColor(renderer, NorthWestX(), NorthWestY(), W(),   0,  90, colour); break;
          case Compass::NORTHWEST: filledPieColor(renderer, SouthEastX(), SouthEastY(), W(), 180, 270, colour); break;
          case Compass::SOUTHWEST: filledPieColor(renderer, NorthEastX(), NorthEastY(), W(),  90, 180, colour); break;
        }
      }

      virtual void Bullnose(SDL_Renderer* renderer, Compass orientation, Colour colour) const {
        switch (orientation) {
          case Compass::NORTH:
            Align(Compass::NORTH, Size2D{W()}).Ellipse(renderer, colour);
            Align(Compass::SOUTH, Size2D{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case Compass::EAST:
            Align(Compass::EAST, Size2D{H()}).Ellipse(renderer, colour);
            Align(Compass::WEST, Size2D{W() - (H() / 2), H()}).Fill(renderer, colour);
            break;

          case Compass::SOUTH:
            Align(Compass::SOUTH, Size2D{W()}).Ellipse(renderer, colour);
            Align(Compass::NORTH, Size2D{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case Compass::WEST:
            Align(Compass::WEST, Size2D{H()}).Ellipse(renderer, colour);
            Align(Compass::EAST, Size2D{W() - (H() / 2), H()}).Fill(renderer, colour);
            break;
        }
      }

      virtual void Draw(SDL_Renderer* renderer) const {
        filledCircleColor(renderer,    CentreX(),    CentreY(), 6, 0x99000000);
        filledCircleColor(renderer,     NorthX(),     NorthY(), 4, 0x99ff0000);
        filledCircleColor(renderer, NorthEastX(), NorthEastY(), 4, 0x9900ffff);
        filledCircleColor(renderer,      EastX(),      EastY(), 4, 0x99ff0000);
        filledCircleColor(renderer, SouthEastX(), SouthEastY(), 4, 0x99ffff00);
        filledCircleColor(renderer,     SouthX(),     SouthY(), 4, 0x9900ff00);
        filledCircleColor(renderer, SouthWestX(), SouthWestY(), 4, 0x9900ffff);
        filledCircleColor(renderer,      WestX(),      WestY(), 4, 0x9900ff00);
        filledCircleColor(renderer, NorthWestX(), NorthWestY(), 4, 0x99ffff00);

        lineColor(renderer,
            NorthWestX(), NorthWestY(),
            NorthEastX(), NorthEastY(),
            0x99ffffff
          );
        lineColor(renderer,
            SouthWestX(), SouthWestY(),
            SouthEastX(), SouthEastY(),
            0x99ffffff
          );
        lineColor(renderer,
            NorthEastX(), NorthEastY(),
            SouthEastX(), SouthEastY(),
            0x99ffffff
          );
        lineColor(renderer,
            NorthWestX(), NorthWestY(),
            SouthWestX(), SouthWestY(),
            0x99ffffff
          );
        lineColor(renderer,
            NorthWestX(), NorthWestY(),
            SouthEastX(), SouthEastY(),
            0x99ffffff
          );
        lineColor(renderer,
            NorthEastX(), NorthEastY(),
            SouthWestX(), SouthWestY(),
            0x99ffffff
          );
      }
  };

}
