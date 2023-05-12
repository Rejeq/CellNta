#pragma once

#include <Eigen/Core>
#include <vector>

#include <Cellnta/Cell.h>
#include <Cellnta/CellForest.h>

namespace Cellnta {

class NCellStorage {
 public:
  using Point = float;
  using Cell = CellEx<Point>;
  using Vec = Eigen::Vector<Point, Eigen::Dynamic>;
  using VecList = CellForest<Cell>;

  bool NeedUpdate() const { return m_needUpdate; }
  void Handled() { m_needUpdate = false; }

  void Restore();

  void Add(const Vec& pos);
  void AddHomogeneous(const Vec& pos);

  void Erase(const Vec& pos);
  void EraseArea(const Area& area);
  void EraseHomogeneous(const Vec& pos);

  void SetDimension(int dim);

  void Clear();
  int Size() const;
  int Capacity() const;

  int GetDimensions() const { return m_d; }

  const VecList& GetRaw() const { return m_cells; }
  VecList& GetRaw() { return m_cells; }

  const VecList& GetVisibleRaw() const { return m_visibleCells; }
  VecList& GetVisibleRaw() { return m_visibleCells; }

 private:
  VecList m_cells;
  VecList m_visibleCells;

  bool m_needUpdate = false;
  int m_d = 0;
};

}  // namespace Cellnta
