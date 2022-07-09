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
      vector add(vector c) const {
        return vector{ x + c.x, y + c.y };
      }

      vector subtract(vector c) const {
        return vector{ x - c.x, y - c.y };
      }

      int centre_x()    const { return x / 2; }
      int centre_y()    const { return y / 2; }
      int north_x()     const { return x / 2; }
      int north_y()     const { return 0;     }
      int east_x()      const { return x;     }
      int east_y()      const { return y / 2; }
      int south_x()     const { return x / 2; }
      int south_y()     const { return y;     }
      int west_x()      const { return 0;     }
      int west_y()      const { return y / 2; }
      int northeast_x() const { return x;     }
      int northeast_y() const { return 0;     }
      int southeast_x() const { return x;     }
      int southeast_y() const { return y;     }
      int southwest_x() const { return 0;     }
      int southwest_y() const { return y;     }
      int northwest_x() const { return 0;     }
      int northwest_y() const { return 0;     }

      vector centre()    const { return vector{ centre_x(),    centre_y()    }; }
      vector north()     const { return vector{ north_x(),     north_y()     }; }
      vector east()      const { return vector{ east_x(),      east_y()      }; }
      vector south()     const { return vector{ south_x(),     south_y()     }; }
      vector west()      const { return vector{ west_x(),      west_y()      }; }
      vector northeast() const { return vector{ northeast_x(), northeast_y() }; }
      vector southeast() const { return vector{ southeast_x(), southeast_y() }; }
      vector southwest() const { return vector{ southwest_x(), southwest_y() }; }
      vector northwest() const { return vector{ northwest_x(), northwest_y() }; }
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
      virtual Position2D origin() const { return _position; };
      virtual vector     size()   const { return _size; };


      // X, Y, W, H shortcuts
      int X() const { return origin().x; }
      int Y() const { return origin().y; }
      int W() const { return size().x; }
      int H() const { return size().y; }


      // Near and Far Point Positions
      Position2D Near() const { return origin(); }
            int near_x() const { return origin().x; }
            int near_y() const { return origin().y; }

      Position2D Far() const { return origin().add(size()); }
            int far_x() const { return Far().x; }
            int far_y() const { return Far().y; }

      // compass points
      Position2D centre()     const { return point(compass::centre   ); }
      Position2D north()      const { return point(compass::north    ); }
      Position2D northeast()  const { return point(compass::northeast); }
      Position2D east()       const { return point(compass::east     ); }
      Position2D southeast()  const { return point(compass::southeast); }
      Position2D south()      const { return point(compass::south    ); }
      Position2D southwest()  const { return point(compass::southwest); }
      Position2D west()       const { return point(compass::west     ); }
      Position2D northwest()  const { return point(compass::northwest); }

      int centre_x()    const { return centre().x;    }
      int centre_y()    const { return centre().y;    }
      int north_x()     const { return north().x;     }
      int north_y()     const { return north().y;     }
      int east_x()      const { return east().x;      }
      int east_y()      const { return east().y;      }
      int south_x()     const { return south().x;     }
      int south_y()     const { return south().y;     }
      int west_x()      const { return west().x;      }
      int west_y()      const { return west().y;      }
      int northeast_x() const { return northeast().x; }
      int northeast_y() const { return northeast().y; }
      int southeast_x() const { return southeast().x; }
      int southeast_y() const { return southeast().y; }
      int southwest_x() const { return southwest().x; }
      int southwest_y() const { return southwest().y; }
      int northwest_x() const { return northwest().x; }
      int northwest_y() const { return northwest().y; }

      // compass Points
      Position2D point(compass align) const {
        switch (align) {
          case    compass::centre: return origin().add(size().centre());
          case     compass::north: return origin().add(size().north());
          case compass::northeast: return origin().add(size().northeast());
          case      compass::east: return origin().add(size().east());
          case compass::southeast: return origin().add(size().southeast());
          case     compass::south: return origin().add(size().south());
          case compass::southwest: return origin().add(size().southwest());
          case      compass::west: return origin().add(size().west());
          case compass::northwest: return origin().add(size().northwest());
        }
        return Position2D();
      }

      // compass Alignment
      Region2D align(compass align, vector s) const {
        switch (align) {
          case    compass::centre: return Region2D{    centre().subtract(s.centre()   ), s };
          case     compass::north: return Region2D{     north().subtract(s.north()    ), s };
          case compass::northeast: return Region2D{ northeast().subtract(s.northeast()), s };
          case      compass::east: return Region2D{      east().subtract(s.east()     ), s };
          case compass::southeast: return Region2D{ southeast().subtract(s.southeast()), s };
          case     compass::south: return Region2D{     south().subtract(s.south()    ), s };
          case compass::southwest: return Region2D{ southwest().subtract(s.southwest()), s };
          case      compass::west: return Region2D{      west().subtract(s.west()     ), s };
          case compass::northwest: return Region2D{ northwest().subtract(s.northwest()), s };
        }
        return Region2D();
      }

      // compass Quadrants
      Region2D northeastQuadrant() const { return Region2D{ north(),       east() }; }
      Region2D southeastQuadrant() const { return Region2D{ centre(), southeast() }; }
      Region2D southwestQuadrant() const { return Region2D{ west(),       south() }; }
      Region2D northwestQuadrant() const { return Region2D{ northwest(), centre() }; }

      // Halfs
      Region2D TopHalf()    const { return Region2D{ northwest(), east()      }; }
      Region2D BottomHalf() const { return Region2D{ west(),      southeast() }; }
      Region2D LeftHalf()   const { return Region2D{ northwest(), south()     }; }
      Region2D RightHalf()  const { return Region2D{ north(),     southeast() }; }

      bool Encloses(vector point) const {
        return ( near_x() <= point.x )
            && ( near_y() <= point.y )
            && ( point.x <= far_x()  )
            && ( point.y <= far_y()  );
      }

      bool Encloses(Region2D r) const {
        return Encloses(r.Near())
            && Encloses(r.Far());
      }

      virtual void Fill(SDL_Renderer* renderer, theme::colour colour) const {
        boxColor(renderer, near_x(), near_y(), far_x(), far_y(), colour);
      }

      virtual void RoundedFill(SDL_Renderer* renderer, int radius, theme::colour colour) const {
        roundedBoxColor(renderer, near_x(), near_y(), far_x(), far_y(), radius, colour);
      }

      virtual void Ellipse(SDL_Renderer* renderer, theme::colour colour) const {
        filledEllipseColor(renderer,
            centre_x(), centre_y(),
            W()/2, H()/2,
            colour
          );
      }

      virtual void Stroke(SDL_Renderer* renderer, theme::colour colour) const {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        boxColor(renderer, near_x()  , near_y()  , far_x()  , far_y()  , colour);
        boxColor(renderer, near_x()+1, near_y()+1, far_x()-2, far_y()-2, 0xff000000);
      }

      virtual void QuadrantArc(SDL_Renderer* renderer, compass orientation, theme::colour colour) const {
        switch (orientation) {
          case compass::northeast: filledPieColor(renderer, southwest_x(), southwest_y(), W(), 270,   0, colour); break;
          case compass::southeast: filledPieColor(renderer, northwest_x(), northwest_y(), W(),   0,  90, colour); break;
          case compass::northwest: filledPieColor(renderer, southeast_x(), southeast_y(), W(), 180, 270, colour); break;
          case compass::southwest: filledPieColor(renderer, northeast_x(), northeast_y(), W(),  90, 180, colour); break;
        }
      }

      virtual void Bullnose(SDL_Renderer* renderer, compass orientation, theme::colour colour) const {
        switch (orientation) {
          case compass::north:
            align(compass::north, vector{W()}).Ellipse(renderer, colour);
            align(compass::south, vector{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case compass::east:
            align(compass::east, vector{H()}).Ellipse(renderer, colour);
            align(compass::west, vector{W() - (H() / 2), H()}).Fill(renderer, colour);
            break;

          case compass::south:
            align(compass::south, vector{W()}).Ellipse(renderer, colour);
            align(compass::north, vector{W(), H() - (W() / 2)}).Fill(renderer, colour);
            break;

          case compass::west:
            align(compass::west, vector{H()}).Ellipse(renderer, colour);
            align(compass::east, vector{W() - (H() / 2), H()}).Fill(renderer, colour);
            break;
        }
      }

      virtual void Draw(SDL_Renderer* renderer) const {
        filledCircleColor(renderer,    centre_x(),    centre_y(), 6, 0x99000000);
        filledCircleColor(renderer,     north_x(),     north_y(), 4, 0x99ff0000);
        filledCircleColor(renderer, northeast_x(), northeast_y(), 4, 0x9900ffff);
        filledCircleColor(renderer,      east_x(),      east_y(), 4, 0x99ff0000);
        filledCircleColor(renderer, southeast_x(), southeast_y(), 4, 0x99ffff00);
        filledCircleColor(renderer,     south_x(),     south_y(), 4, 0x9900ff00);
        filledCircleColor(renderer, southwest_x(), southwest_y(), 4, 0x9900ffff);
        filledCircleColor(renderer,      west_x(),      west_y(), 4, 0x9900ff00);
        filledCircleColor(renderer, northwest_x(), northwest_y(), 4, 0x99ffff00);

        lineColor(renderer,
            northwest_x(), northwest_y(),
            northeast_x(), northeast_y(),
            0x99ffffff
          );
        lineColor(renderer,
            southwest_x(), southwest_y(),
            southeast_x(), southeast_y(),
            0x99ffffff
          );
        lineColor(renderer,
            northeast_x(), northeast_y(),
            southeast_x(), southeast_y(),
            0x99ffffff
          );
        lineColor(renderer,
            northwest_x(), northwest_y(),
            southwest_x(), southwest_y(),
            0x99ffffff
          );
        lineColor(renderer,
            northwest_x(), northwest_y(),
            southeast_x(), southeast_y(),
            0x99ffffff
          );
        lineColor(renderer,
            northeast_x(), northeast_y(),
            southwest_x(), southwest_y(),
            0x99ffffff
          );
      }
  };

}
