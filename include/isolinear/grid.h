#pragma once

#include <list>
#include <exception>
#include <utility>

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#include "geometry.h"



namespace isolinear {


  class grid {

    protected:
      geometry::vector m_cell_size{0};
      geometry::vector m_size{0};
      geometry::vector m_gutter{0};
      geometry::region m_bounds{0};
      geometry::vector m_offset{0};

    public:

      grid(
          geometry::region b,
          geometry::vector cs,
          geometry::vector g,
          geometry::vector s,
          geometry::vector o
        ) :
          m_bounds{std::move(b)},
          m_cell_size{cs},
          m_gutter{g},
          m_size{s},
          m_offset{o}
      { };

      grid subgrid(
        int near_col, int near_row,
        int  far_col, int  far_row
      ) const {
        return grid{
            calculate_grid_region(
                near_col, near_row,
                far_col, far_row
              ),
            m_cell_size,
            m_gutter,
            geometry::vector(
                far_col - near_col + 1,
                far_row - near_row + 1
              ),
            {0}
          };
      }

      grid row(int row) const {
        int cols = max_columns();
        int rows = max_rows();

        row = (row > 0) ? row : rows + row;

        return subgrid(1, row, cols, row);
      }

      grid rows(int from, int to) const {
        int cols = max_columns();
        int rows = max_rows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return subgrid(1, from, cols, to);
      }

      grid column(int col) const {
        int cols = max_columns();
        int rows = max_rows();

        col = (col > 0) ? col : cols + col;

        return subgrid(col, 1, col, rows);
      }

      grid columns(int from, int to) const {
        int cols = max_columns();
        int rows = max_rows();

        from = (from > 0) ? from : rows + from;
          to = (  to > 0) ?   to : rows +   to;

        return subgrid(from, 1, to, rows);
      }

      grid left_columns(int n) const {
        return columns(1, n);
      }

      grid right_columns(int n) const {
        return columns(max_columns() - n + 1, max_columns());
      }

      grid centre_columns(int l, int r) const {
        return columns(l + 1, max_columns() - r);
      }

      grid top_rows(int n) const {
        return rows(1, n);
      }

      grid bottom_rows(int n) const {
        return rows(max_rows() - (n-1), max_rows());
      }

      grid centre_rows(int t, int b) const {
        return rows(t + 1, max_rows() - b);
      }

      geometry::region cell(int col, int row) const {
        return calculate_grid_region(col, row, col, row);
      }

      geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
        auto mc = max_columns();
        auto mr = max_rows();

        if (false) if (near_col > mc || far_col > mc || near_row > mr || far_row > mr) {
          throw std::out_of_range(fmt::format(
              "Grid region {},{} x {},{} falls outside of the grid bounds, {} x {}",
              near_col, near_row, far_col, far_row, mc, mr));
        }

        geometry::position origin = m_bounds.origin().add(m_offset);
        return geometry::region{
          /* x */ origin.x + (m_cell_size.x * (near_col - 1)),
          /* y */ origin.y + (m_cell_size.y * (near_row - 1)),
          /* w */ m_cell_size.x * ((far_col - near_col) + 1) - m_gutter.x,
          /* h */ m_cell_size.y * ((far_row - near_row) + 1) - m_gutter.y
        };
      }

      void draw(SDL_Renderer* renderer) const {
        for (int i=1; i<=max_columns(); i++) {
          for (int j=1; j<=max_rows(); j++) {
            cell(i, j).fill(renderer, 0x33ffffff);
          }
        }
      }

      int position_column_index(geometry::position p) const {
        return floor((p.x - (m_bounds.X() + m_offset.x)) / m_cell_size.x) + 1;
      }

      int position_row_index(geometry::position p) const {
        return floor((p.y - (m_bounds.Y() + m_offset.y)) / m_cell_size.y) + 1;
      }

      int max_columns() const {
        return m_size.x;
      }

      int max_rows() const {
        return m_size.y;
      }

      geometry::vector gutter() const {
        return m_gutter;
      }

      geometry::region bounds() const {
        return m_bounds;
      }

      void print() const {
        printf("Grid %d,%d (%d,%d)\n",
            m_bounds.X(), m_bounds.Y(),
            m_bounds.W(), m_bounds.H()
          );
      }
  };


    class gridfactory {
    protected:
        geometry::vector m_cell_size{100, 50};
        geometry::vector m_size{3,3};
        geometry::vector m_gutter{0};
        geometry::region m_bounds;
        geometry::vector m_offset{0};
        std::list<isolinear::grid> m_grids{};

    public:
        gridfactory(
            geometry::region b,
            geometry::vector cs,
            geometry::vector g
          )
          : m_bounds(b)
          , m_cell_size(cs)
          , m_gutter(g)
          , m_size( b.W() / cs.x, b.H() / cs.y )
          , m_offset( (b.W() % cs.x) / 2, (b.H() % cs.y) / 2 )
        {};

        geometry::vector size() const {
          return m_size;
        }

        grid& root() {
          return subgrid(1,1, m_size.x, m_size.y);
        }

        grid& subgrid(
            int near_col, int near_row,
            int  far_col, int  far_row
        ) {
          auto region = calculate_grid_region(
              near_col, near_row,
              far_col, far_row
            );
          geometry::vector size(
              far_col - near_col + 1,
              far_row - near_row + 1
            );
          geometry::vector offset(0,0);
          m_grids.emplace_back(region, m_cell_size, m_gutter, size, offset);
          return m_grids.back();
        }

        geometry::region calculate_grid_region(int near_col, int near_row, int far_col, int far_row) const {
          geometry::position origin = m_bounds.origin().add(m_offset);
          return geometry::region{
              /* x */ origin.x + (m_cell_size.x * (near_col - 1)),
              /* y */ origin.y + (m_cell_size.y * (near_row - 1)),
              /* w */ m_cell_size.x * ((far_col - near_col) + 1) - m_gutter.x,
              /* h */ m_cell_size.y * ((far_row - near_row) + 1) - m_gutter.y
          };
        }

    };

};
