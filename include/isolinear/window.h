#pragma once

#include "display.h"
#include "geometry.h"
#include "compass.h"


namespace isolinear::window {



  using isolinear::compass;

  using std::min;
  using std::max;


  class position : public geometry::vector {
    protected:
      display::window& window;

    public:
      position(
          display::window& w,
          geometry::vector v
        )
        : vector(v)
        , window(w)
      {}

      position(
          display::window& w,
          int x,
          int y
        )
        : position(w, {x, y})
      {}

    public:
      void draw() const {
        filledEllipseColor(window.renderer(), x, y,  5,  5, 0xff00ffff);
      }
  };

  class region {
    protected:
      display::window& m_window;
      window::position m_near;
      window::position m_far;

    public:
      region(
          display::window& w,
          window::position a,
          window::position b
        )
        : m_window(w)
        , m_near(w, min(a.x, b.x), min(a.y, b.y))
        , m_far(w, max(a.x, b.x), max(a.y, b.y))
      {}

      region(
          display::window& w,
          geometry::vector a,
          geometry::vector b
        )
        : region(w, window::position{w, a}, window::position{w, b})
      {}

      region(
          display::window& w,
          window::position p,
          geometry::vector s
        )
        : region(w, p, window::position{w, p.add(s) })
      {}

      region(display::window& w) : region(w, {w, 0, 0}, {w, 0, 0}) {}

    public: // window::position factories
      window::position position(geometry::vector v) const {
        return window::position(m_window, v);
      }
      window::position position(int x, int y) const {
        return position(geometry::vector(x, y));
      }
      window::position position() const {
        return near();
      }

    public: // Read Accessors
      window::position near() const { return m_near; }
      window::position far()  const { return m_far;  }
      geometry::vector size() const {
        return (m_far.x - m_near.x, m_far.y - m_near.y);
      }

    public: // Shorthand accessors for region geometry
      int x() const { return near().x; };
      int y() const { return near().y; };
      int w() const { return size().x; };
      int h() const { return size().y; };

    public: // Shorthand accessors for defining points
      int nearx() const { return near().x; }
      int neary() const { return near().y; }
      int farx()  const { return far().x;  }
      int fary()  const { return far().y;  }

    public: // Calculate position of a compass point on this region
      window::position point(compass align) const {
        switch (align) {
          case compass::centre:    return position(near().add(size().Centre()   ));
          case compass::north:     return position(near().add(size().North()    ));
          case compass::northeast: return position(near().add(size().NorthEast()));
          case compass::east:      return position(near().add(size().East()     ));
          case compass::southeast: return position(near().add(size().SouthEast()));
          case compass::south:     return position(near().add(size().South()    ));
          case compass::southwest: return position(near().add(size().SouthWest()));
          case compass::west:      return position(near().add(size().West()     ));
          case compass::northwest: return position(near().add(size().NorthWest()));
        }
        return near();
      }

    public: // Shorthand accessors for compass points
      window::position centre()     const { return point(compass::centre   ); }
      window::position north()      const { return point(compass::north    ); }
      window::position northeast()  const { return point(compass::northeast); }
      window::position east()       const { return point(compass::east     ); }
      window::position southeast()  const { return point(compass::southeast); }
      window::position south()      const { return point(compass::south    ); }
      window::position southwest()  const { return point(compass::southwest); }
      window::position west()       const { return point(compass::west     ); }
      window::position northwest()  const { return point(compass::northwest); }

    public: // Shorthand accessors for compass point coordinates
      int centrex()    const { return centre().x;    }
      int centrey()    const { return centre().y;    }
      int northx()     const { return north().x;     }
      int northy()     const { return north().y;     }
      int eastx()      const { return east().x;      }
      int easty()      const { return east().y;      }
      int southx()     const { return south().x;     }
      int southy()     const { return south().y;     }
      int westx()      const { return west().x;      }
      int westy()      const { return west().y;      }
      int northeastx() const { return northeast().x; }
      int northeasty() const { return northeast().y; }
      int southeastx() const { return southeast().x; }
      int southeasty() const { return southeast().y; }
      int southwestx() const { return southwest().x; }
      int southwesty() const { return southwest().y; }
      int northwestx() const { return northwest().x; }

    public: // Compass divisions
      window::region quarter(compass align) const {
        switch (align) {
          case compass::northeast: return window::region(m_window, north(), east());
          case compass::southeast: return window::region(m_window, centre(), southeast());
          case compass::northwest: return window::region(m_window, northwest(), centre());
          case compass::southwest: return window::region(m_window, west(), south());
          default: throw std::runtime_error(fmt::format("Does not name a quadrant: {}", align));
        }
      }
      window::region half(compass align) const {
        switch (align) {
          case compass::north: return window::region(m_window, northwest(), east());
          case compass::south: return window::region(m_window, west(), southeast());
          case compass::east:  return window::region(m_window, north(), southeast());
          case compass::west:  return window::region(m_window, northwest(), south());
          default: throw std::runtime_error(fmt::format("Does not name a half: {}", align));
        }
      }

    public: // Drawing
      virtual void fill(uint32_t colour) const {
        boxColor(m_window.renderer(), m_near.x, m_near.y, m_far.x, m_far.y, colour);
      }

    public: // Debug draw
      uint32_t col;
      void fill_colour(uint32_t c) {
        col = c;
      }
      virtual void draw() const {
        fill(col);
        return;

        centre().draw();
        north().draw();
        northeast().draw();
        east().draw();
        southeast().draw();
        south().draw();
        southwest().draw();
        west().draw();
        northwest().draw();
      }
  };

}
