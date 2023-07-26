#pragma once

#include <Eigen/Core>
#include <vector>

#include <Cellnta/Cell.h>
#include <Cellnta/CellForest.h>
#include <Cellnta/Renderer/ShuffleAxisStorage.h>

namespace Cellnta {

class NCellStorage : public ShuffleAxisStorage {
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

  // Special iterator without shuffling is helpfull - when you need to make
  // projections with shuffling, but present to the screen without shuffling
  //
  // Be carefull when using.
  // For example if you have original position like: (2, 5, 9),
  // and shuffle data like: (0, 2, 1)
  // Visible position will be transformed to: (2, 9, 5)
  // then this iterator returns: (2, 5, 9)
  //
  // But when you have same axis in shuffle data like: (0, 2, 2)
  // Visible position will be tranformed to: (2, 9, 9)
  // then this iterator returns (2, 9, 9)
  // Which may not be what you expect
  //
  // Simillar method for m_cells iterators is not added - because this
  // behaviour can be reached by calling ResetAxisShuffle() before MakeIter()
  IterVisible MakeNoShuffleVisibleIter() const;

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
    Iter(const NCellStorage& storage, VecList::WholeIter&& iter,
         Cell::Pos&& pos, int activeAxes)
        : m_iter(iter),
          m_cell({pos, 0}),
          m_activeAxes(activeAxes),
          m_storage(&storage) {
      Reset();
    }
    void Reset() override;
    const Cell* Next() override;

   private:
    VecList::WholeIter m_iter;
    Cell m_cell;
    int m_activeAxes;
    const NCellStorage* m_storage;
  };

  class IterVisible : public IterBase::CellForwardEx<Cell> {
   public:
    IterVisible(const NCellStorage& storage, bool needShuffle)
        : m_storage(&storage), m_needShuffle(needShuffle) {
      Reset();
    }
    void Reset() override;
    const Cell* Next() override;

   private:
    Cell m_cell;
    const NCellStorage* m_storage;
    size_t m_idx;
    bool m_needShuffle;
  };

 private:
  virtual void OnShuffleAxisChanged() { m_needUpdate = true; }

  VecList m_cells;

  // CellForset is not the best choice for storing visible positions.
  // Because positions will be modified many times
  // when projection to a lower dimension happens
  VisibleList m_visible;

  int m_d = 0;
  bool m_needUpdate = false;
};

}  // namespace Cellnta
