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

  // Positions and Cell::State are packed into one list
  // Plus positon is homogeneous
  using VisibleList = std::vector<uint8_t>;

  class Iter;
  class IterVisible;

  bool NeedUpdate() const { return m_needUpdate; }
  void Handled() { m_needUpdate = false; }

  void RestoreVisible();

  void Add(const Vec& pos);

  void Erase(const Vec& pos);
  void EraseArea(const Area& area);

  void SetDimension(int dim);
  int GetDimension() const { return m_d; }

  void ClearVisible();
  void Clear();
  int Size() const;
  int Capacity() const;

  Iter MakeIter() const;
  Iter MakeHomogeneousIter() const;
  IterVisible MakeVisibleIter() const;

  int GetVisiblePosLength() const { return GetDimension() + 1; }
  int GetVisiblePosOffset() const { return 0; }
  int GetVisibleStateOffset() const {
    return GetVisiblePosLength() * sizeof(Point);
  }
  int GetVisibleStride() const {
    return GetVisibleStateOffset() + sizeof(Cell::State);
  }
  int GetVisibleSize() const { return m_visible.size() / GetVisibleStride(); }

  // TODO: Make private access to m_visible
  const uint8_t* GetVisibleRaw() const { return m_visible.data(); }
  uint8_t* GetVisibleRaw() { return m_visible.data(); }

  class Iter : public IterBase::CellForwardEx<Cell> {
   public:
    Iter(VecList::WholeIter&& iter, Cell::Pos&& pos, int activeAxes)
        : m_iter(iter), m_cell({pos, 0}), m_activeAxes(activeAxes) {
      Reset();
    }
    void Reset() override;
    const Cell* Next() override;

   private:
    VecList::WholeIter m_iter;
    Cell m_cell;
    int m_activeAxes;
  };

  class IterVisible : public IterBase::CellForwardEx<Cell> {
   public:
    IterVisible(const NCellStorage& storage) : m_storage(&storage) { Reset(); }
    void Reset() override;
    const Cell* Next() override;

   private:
    Cell m_cell;
    const NCellStorage* m_storage;
    size_t m_idx;
  };

 private:
  VecList m_cells;

  // CellForset is not the best choice for storing visible positions.
  // Because positions will be modified many times
  // when projection to a lower dimension happens
  VisibleList m_visible;

  bool m_needUpdate = false;
  int m_d = 0;
};

}  // namespace Cellnta
