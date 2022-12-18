#pragma once

#include <Eigen/Core>
#include <vector>

#include "Cellnta/Config.h"

namespace Cellnta {

class NCellStorage {
 public:
  using Point = float;
  using Vec = Eigen::Vector<Point, Eigen::Dynamic>;
  using VecList = std::vector<Vec>;

  void Restore();

  template <bool homogeneous, typename Derived>
  void Add(const Derived& pos);

  void SetDimension(int dim);

  void clear();
  void reserve(int capacity);
  size_t capacity() const;
  size_t size() const;

  int GetDimensions() const { return m_d; }

  const VecList& GetRaw() const { return m_cells; }
  VecList& GetRaw() { return m_cells; }

  const VecList& GetOrigRaw() const { return m_origCells; }
  VecList& GetOrigRaw() { return m_origCells; }

 private:
  VecList m_cells;
  VecList m_origCells;

  int m_d = 0;
};

template <bool homogeneous, typename Derived>
void NCellStorage::Add(const Derived& pos) {
  CELLNTA_PROFILE;

  using Scalar = typename Derived::Scalar;

  if constexpr (homogeneous) {
    assert(pos.size() >= m_d + 1);
    if constexpr (std::is_same<Scalar, Point>::value)
      m_cells.push_back(pos);
    else
      m_cells.push_back(pos.template cast<Point>());
  } else {
    assert(pos.size() >= m_d);
    Eigen::Vector<Scalar, Eigen::Dynamic> tmp(m_d + 1);
    memcpy(tmp.data(), pos.data(), (m_d) * sizeof(Scalar));
    tmp(m_d) = 1;

    if constexpr (std::is_same<Scalar, Point>::value)
      m_cells.push_back(tmp);
    else
      m_cells.push_back(tmp.template cast<Point>());
  }
  m_origCells.emplace_back(m_cells.at(m_cells.size() - 1));
}

}  // namespace Cellnta
