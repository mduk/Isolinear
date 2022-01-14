#pragma once

#include <stdexcept>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "compass.h"
#include "theme.h"

using isolinear::compass;

namespace isolinear::geometry {

  class vector {
    public:
      int x, y;

      vector(int _x, int _y) : x{_x}, y{_y} {};
      vector(int x) : vector(x, x) {};
      vector() : vector(0, 0) {}

      //vector(vector c) : vector{c} {}

      vector(SDL_MouseButtonEvent e) : x{e.x}, y{e.y} {};
      vector(SDL_MouseMotionEvent e) : x{e.x}, y{e.y} {};

      vector(SDL_Surface* s) : x{s->w}, y{s->h} {};

      //vector(SDL_Rect r) : vector(r.x, r.y) {};
      //vector(SDL_Rect r) : vector(r.w, r.h) {};


    public:
      vector Add(vector c) const {
        return vector{ x + c.x, y + c.y };
      }

      vector Subtract(vector c) const {
        return vector{ x - c.x, y - c.y };
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

      vector Centre()    const { return vector{ CentreX(),    CentreY()    }; }
      vector North()     const { return vector{ NorthX(),     NorthY()     }; }
      vector East()      const { return vector{ EastX(),      EastY()      }; }
      vector South()     const { return vector{ SouthX(),     SouthY()     }; }
      vector West()      const { return vector{ WestX(),      WestY()      }; }
      vector NorthEast() const { return vector{ NorthEastX(), NorthEastY() }; }
      vector SouthEast() const { return vector{ SouthEastX(), SouthEastY() }; }
      vector SouthWest() const { return vector{ SouthWestX(), SouthWestY() }; }
      vector NorthWest() const { return vector{ NorthWestX(), NorthWestY() }; }
  };


  class Position2D : public vector {
    public:
      Position2D() : vector() {}
      Position2D(vector c) : vector{c} {}
      Position2D(int x, int y) : vector(x, y) {};
      Position2D(SDL_MouseButtonEvent e) : vector(e) {};
      Position2D(SDL_Rect r) : vector(r.x, r.y) {};

      void Draw(SDL_Renderer* renderer) {
        filledEllipseColor(renderer, x, y,  5,  5, 0xff00ffff);
      }
  };

  class Region2D {
    protected:
      Position2D _position;
      vector _size;

    public:
      Region2D()
          : _position{0,0}, _size{0,0}
        {};

      Region2D(vector s)
          : _position{0,0}, _size{s}
        {};

      Region2D(SDL_Rect r)
          : _position{r.x, r.y}, _size{r.w, r.h}
        {};

      Region2D(Position2D _p, vector _s)
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
          _size = vector{
              far.x - near.x,
              far.y - near.y
            };
        };

      Region2D(int _x, int _y, int _w, int _h)
          : _position{_x, _y}, _size{_w, _h}
        {};


      void Resize(vector newsize) {
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
      virtual vector     Size()   const { return _size; };


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

      // compass points
      Position2D Centre()     const { return Point(compass::centre   ); }
      Position2D North()      const { return Point(compass::north    ); }
      Position2D NorthEast()  const { return Point(compass::northeast); }
      Position2D East()       const { return Point(compass::east     ); }
      Position2D SouthEast()  const { return Point(compass::southeast); }
      Position2D South()      const { return Point(compass::south    ); }
      Position2D SouthWest()  const { return Point(compass::southwest); }
      Position2D West()       const { return Point(compass::west     ); }
      Position2D NorthWest()  const { return Point(compass::northwest); }

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

      // compass Points
      Position2D Point(compass align) const {
        switch (align) {
          case    compass::centre: return Origin().Add(Size().Centre());
          case     compass::north: return Origin().Add(Size().North());
          case compass::northeast: return Origin().Add(Size().NorthEast());
          case      compass::east: return Origin().Add(Size().East());
          case compass::southeast: return Origin().Add(Size().SouthEast());
          case     compass::south: return Origin().Add(Size().South());
          case compass::southwest: return Origin().Add(Size().SouthWest());
          case      compass::west: return Origin().Add(Size().West());
          case compass::northwest: return Origin().Add(Size().NorthWest());
        }
        return Position2D();
      }

      // compass Alignment
      Region2D Align(compass align, vector s) const {
        switch (align) {
          case    compass::centre: return Region2D{    Centre().Subtract(s.Centre()   ), s };
          case     compass::north: return Region2D{     North().Subtract(s.North()    ), s };
          case compass::northeast: return Region2D{ NorthEast().Subtract(s.NorthEast()), s };
          case      compass::east: return Region2D{      East().Subtract(s.East()     ), s };
          case compass::southeast: return Region2D{ SouthEast().Subtract(s.SouthEast()), s };
          case     compass::south: return Region2D{     South().Subtract(s.South()    ), s };
          case compass::southwest: return Region2D{ SouthWest().Subtract(s.SouthWest()), s };
          case      compass::west: return Region2D{      West().Subtract(s.West()     ), s };
          case compass::northwest: return Region2D{ NorthWest().Subtract(s.NorthWest()), s };
        }
        return Region2D();
      }

      // compass Quadrants
      Region2D NorthEastQuadrant() const { return Region2D{ North(),       East() }; }
      Region2D SouthEastQuadrant() const { return Region2D{ Centre(), SouthEast() }; }
      Region2D SouthWestQuadrant() const { return Region2D{ West(),       South() }; }
      Region2D NorthWestQuadrant() const { return Region2D{ NorthWest(), Centre() }; }

      // Halfs
      Region2D TopHalf()    const { return Region2D{ NorthWest(), East()      }; }
      Region2D BottomHalf() const { return Region2D{ West(),      SouthEast() }; }
      Region2D LeftHalf()   const { return Region2D{ NorthWest(), South()     }; }
      Region2D RightHalf()  const { return Region2D{ North(),     SouthEast() }; }

      bool Encloses(vector point) const {
        return ( NearX() <= point.x )
            && ( NearY() <= point.y )
            && ( point.x <= FarX()  )
            && ( point.y <= FarY()  );
      }

      bool Encloses(Region2D r) const {
        return Encloses(r.Near())
            && Encloses(r.Far());
      }

      virtual void Fill(SDL_Renderer* renderer, theme::colour colour) const {
        boxColor(renderer, NearX(), NearY(), FarX(), FarY(), colour);
      }

      virtual void RoundedFill(SDL_Renderer* renderer, int radius, theme::colour colour) const {
        roundedBoxColor(renderer, NearX(), NearY(), FarX(), FarY(), radius, colour);
      }

      virtual void Ellipse(SDL_Renderer* renderer, theme::colour colour) const {
        filledEllipseColor(renderer,
            CentreX(), CentreY(),
            W()/2, H()/2,
            colour
          );
      }

      virtual void Stroke(SDL_Renderer* renderer, theme::colour colour) const {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        boxColor(renderer, NearX()  , NearY()  , FarX()  , FarY()  , colour);
        boxColor(renderer, NearX()+1, NearY()+1, FarX()-2, FarY()-2, 0xff000000);
      }

      virtual void QuadrantArc(SDL_Renderer* renderer, compass orientation, theme::colour colour) const {
        switch (orientation) {
          case compass::northeast: filledPieColor(renderer, SouthWestX(), SouthWestY(), W(), 270,   0, colour); break;
          case compass::southeast: filledPieColor(renderer, NorthWestX(), NorthWestY(), W(),   0,  90, colour); break;
          case compass::northwest: filledPieColor(renderer, SouthEastX(), SouthEastY(), W(), 180, 270, colour); break;
          case compass::southwest: filledPieColor(renderer, NorthEastX(), NorthEastY(), W(),  90, 180, colour); break;
        }
      }

      virtual void Bullnose(SDL_Renderer* renderer, compass orientation, theme::colour colour) const {
        switch (orientation) {
          case compass::north:
            Align(compass::north, vector{W()}).Ellipse(renderer, colour);
            Align(compass::south, vector{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case compass::east:
            Align(compass::east, vector{H()}).Ellipse(renderer, colour);
            Align(compass::west, vector{W() - (H() / 2), H()}).Fill(renderer, colour);
            break;

          case compass::south:
            Align(compass::south, vector{W()}).Ellipse(renderer, colour);
            Align(compass::north, vector{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case compass::west:
            Align(compass::west, vector{H()}).Ellipse(renderer, colour);
            Align(compass::east, vector{W() - (H() / 2), H()}).Fill(renderer, colour);
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
