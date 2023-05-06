#include "Utils.h"

#include <Cellnta/Axis.h>

#include <cassert>

std::vector<Cellnta::Cell> GenerateCellList(const Cellnta::Area& area) {
  if (!IsAreaHasEqualValues(area)) {
    assert(false && "GenerateCellList should have equal values, because "
                          "I am too lazy to implement this function properly");
    return {};
  }

  std::vector<Cellnta::Cell> res;

  for (int i = area.MinAxis(0); Cellnta::Axis::WithinUpperBound(area.MaxAxis(0), i); ++i) {
    Cellnta::Cell cell;
    cell.pos = Eigen::Vector3i::Constant(i);
    cell.state = i;
    res.push_back(cell);
  }

  return res;
}
