#pragma once

#include <vector>

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

struct Area {
  Eigen::Vector3i min;
  Eigen::Vector3i max;

  Area();
  Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max);
  Area(int min, int max);

  bool Valid() const;
  bool PosValid(const Cell::Pos& pos) const;
  int GetSize() const { return min.size(); }

  static Area Intersection(const Area& first, const Area& second);
  static std::vector<Area> InverseClip(const Area& clipper,
                                       const Area& subject);

  Area operator+(const Area& r) const;
  Area operator+(const Eigen::Vector3i& r) const;
  Area operator-(const Area& r) const;
  Area operator-(const Eigen::Vector3i& r) const;

  bool operator==(const Area& r) const;
  bool operator!=(const Area& r) const;
};

}  // namespace Cellnta

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
