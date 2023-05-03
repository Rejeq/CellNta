#pragma once

#include <cstdint>

#include <Eigen/Core>

namespace Cellnta {

struct Cell {
  using Point = int32_t;
  using State = uint8_t;
  using Pos = Eigen::Vector<Point, Eigen::Dynamic>;

  Pos pos;
  State state = 0;

  Cell() = default;
  Cell(const Pos& pos, State state) : pos(pos), state(state) {}

  bool operator==(const Cell& r) const;
  bool operator!=(const Cell& r) const;
};

}

