#pragma once

#include <vector>

#include <Cellnta/Area.h>

inline bool IsPosHasEqualValues(const Eigen::Vector3i& pos) {
  auto it = std::adjacent_find(pos.begin(), pos.end(), std::equal_to<int>());
  return it != pos.end();
}

inline bool IsAreaHasEqualValues(const Cellnta::Area& area) {
  return IsPosHasEqualValues(area.Min()) && IsPosHasEqualValues(area.Max());
}

std::vector<Cellnta::Cell> GenerateCellList(const Cellnta::Area& area);
