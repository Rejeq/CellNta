#include "Utils.h"

#include <cassert>

std::vector<Cellnta::Cell> GenerateCellList(const Cellnta::Area& area) {
  if (!IsAreaHasEqualValues(area)) {
    assert(false && "GenerateCellList should have equal values, because "
                          "I am too lazy to implement this function properly");
    return {};
  }

  std::vector<Cellnta::Cell> res;

  for (int i = area.min[0]; i < area.max[0]; ++i) {
    Cellnta::Cell cell;
    cell.pos = Eigen::Vector3i::Constant(i);
    cell.state = i;
    res.push_back(cell);
  }

  return res;
}
