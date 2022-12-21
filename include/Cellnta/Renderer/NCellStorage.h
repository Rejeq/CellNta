#pragma once

#include <Eigen/Core>
#include <vector>

namespace Cellnta {

class NCellStorage {
 public:
  using Point = float;
  using Vec = Eigen::Vector<Point, Eigen::Dynamic>;
  using VecList = std::vector<Vec>;

  void Restore();

  void Add(const Vec& pos);
  void AddHomogeneous(const Vec& pos);

  void SetDimension(int dim);

  void clear();
  void reserve(int capacity);
  size_t capacity() const;
  size_t size() const;

  int GetDimensions() const { return m_d; }

  const VecList& GetRaw() const { return m_cells; }
  VecList& GetRaw() { return m_cells; }

  const VecList& GetVisibleRaw() const { return m_visibleCells; }
  VecList& GetVisibleRaw() { return m_visibleCells; }

 private:
  VecList m_cells;
  VecList m_visibleCells;

  int m_d = 0;
};

}  // namespace Cellnta
