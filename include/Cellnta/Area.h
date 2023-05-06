#pragma once

#include <vector>

#include <Eigen/Core>

#include "Cellnta/Cell.h"

namespace Cellnta {

struct Area {
  Area();
  Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max);
  Area(int min, int max);

  bool Valid() const;
  bool PosValid(const Cell::Pos& pos) const;
  int GetSize() const;

  static Area Intersection(const Area& first, const Area& second);
  static std::vector<Area> InverseClip(const Area& clipper,
                                       const Area& subject);

  Eigen::Vector3i& Min() { return m_min; }
  const Eigen::Vector3i& Min() const { return m_min; }
  bool SetMinAxis(int idx, int val);
  int MinAxis(int idx) const;

  Eigen::Vector3i& Max() { return m_max; }
  const Eigen::Vector3i& Max() const { return m_max; }
  bool SetMaxAxis(int idx, int val);
  int MaxAxis(int idx) const;

  Area operator+(const Area& r) const;
  Area operator+(const Eigen::Vector3i& r) const;
  Area operator-(const Area& r) const;
  Area operator-(const Eigen::Vector3i& r) const;

  bool operator==(const Area& r) const;
  bool operator!=(const Area& r) const;

 private:
  Eigen::Vector3i m_min;
  Eigen::Vector3i m_max;
};

}  // namespace Cellnta
