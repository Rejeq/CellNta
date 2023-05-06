#pragma once

#include <vector>

#include <Eigen/Core>

#include "Cellnta/Cell.h"

namespace Cellnta {

struct Area {
  Eigen::Vector3i min;
  Eigen::Vector3i max;

  Area();
  Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max);
  Area(int min, int max);

  bool Valid() const;
  bool PosValid(const Cell::Pos& pos) const;
  int GetSize() const;

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
