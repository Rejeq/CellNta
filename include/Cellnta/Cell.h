#pragma once

#include <cstdint>

#include <Eigen/Core>

namespace Cellnta {

struct Cell {
  using State = uint8_t;
  using Pos = Eigen::Vector<int32_t, Eigen::Dynamic>;

  Pos pos;
  State state = 0;

  Cell() = default;
  Cell(const Pos& pos, State state) : pos(pos), state(state) {}

  bool operator==(const Cell& r) const;
  bool operator!=(const Cell& r) const;
};

}

