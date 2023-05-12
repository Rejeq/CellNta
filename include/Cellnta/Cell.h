#pragma once

#include <cstdint>

#include <Eigen/Core>

namespace Cellnta {

template<typename Scalar = int32_t>
struct CellEx {
  using State = uint8_t;
  using Pos = Eigen::Vector<Scalar, Eigen::Dynamic>;

  Pos pos;
  State state = 0;

  CellEx() = default;
  CellEx(const Pos& pos, State state) : pos(pos), state(state) {}

  bool operator==(const CellEx& r) const {
    return (pos == r.pos) && (state == r.state);
  }

  bool operator!=(const CellEx& r) const {
    return !(*this == r);
  }
};

typedef CellEx<> Cell;

}

namespace std {

template <>
struct hash<Cellnta::Cell::Pos> {
  using Pos = Cellnta::Cell::Pos;
  size_t operator()(const Pos& data) const {
    int seed = 0;
    for (int i = 0; i < data.size(); ++i)
      seed ^= std::hash<typename Pos::Scalar>()(data(i)) + 0x9e3779b9 +
              (seed << 6) + (seed >> 2);
    return seed;
  }
};

}  // namespace std
