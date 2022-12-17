#pragma once

#include <Eigen/Core>
#include <vector>

#include "Cellnta/Config.h"
#include "Cellnta/Renderer/RenderData.h"

namespace Cellnta {

class NCellStorage {
 public:
  using Point = float;
  using Vec = Eigen::Vector<Point, Eigen::Dynamic>;
  using VecList = std::vector<Vec>;

  template <bool homogeneous, typename Derived>
  bool Add(const Derived& pos);
  void Restore();

  void SetDimensions(const int dim) {
    m_d = dim;
    clear();
  }
  template <bool Clear, bool Reserve>
  void SetMaxCellsSize(const int maxCells) {
    m_maxCells = maxCells;
    if constexpr (Clear)
      clear();
    if constexpr (Reserve)
      reserve(m_maxCells);
  }

  const Vec& OriginalAt(const int idx) const { return m_origCells.at(idx); }

  int GetDimensions() const { return m_d; }
  int PointsCapacity3dInBytes() const {
    return capacity() * sizeof(point_t) * 3;
  }

  void clear() {
    m_cells.clear();
    m_origCells.clear();
  };
  void reserve(int capacity) {
    m_cells.reserve(capacity);
    m_origCells.reserve(capacity);
  };
  int capacity() const { return m_cells.capacity(); }

  const Vec* data() const { return m_cells.data(); }
  Vec* data() { return m_cells.data(); }
  const Vec& at(const int idx) const { return m_cells.at(idx); }
  Vec& at(const int idx) { return m_cells.at(idx); }
  int size() const { return m_cells.size(); }

 private:
  std::vector<Vec> m_cells;
  std::vector<Vec> m_origCells;

  int m_maxCells = 0;
  int m_d = 0;
};

template <bool homogeneous, typename Derived>
bool NCellStorage::Add(const Derived& pos) {
  CELLNTA_PROFILE;

  using Scalar = typename Derived::Scalar;
  const size_t oldCapacity = m_cells.capacity();

  if constexpr (homogeneous) {
    assert(pos.size() >= m_d + 1);
    if constexpr (std::is_same<Scalar, point_t>::value)
      m_cells.push_back(pos);
    else
      m_cells.push_back(pos.template cast<point_t>());
  } else {
    assert(pos.size() >= m_d);
    Eigen::Vector<Scalar, Eigen::Dynamic> tmp(m_d + 1);
    memcpy(tmp.data(), pos.data(), (m_d) * sizeof(Scalar));
    tmp(m_d) = 1;

    if constexpr (std::is_same<Scalar, point_t>::value)
      m_cells.push_back(tmp);
    else
      m_cells.push_back(tmp.template cast<point_t>());
  }
  m_origCells.emplace_back(m_cells.at(m_cells.size() - 1));

  return (oldCapacity != m_cells.capacity());
}

}  // namespace Cellnta
