#pragma once

#include "Cellnta/Adjustable.h"
#include "Cellnta/Area.h"
#include "Cellnta/Iterator.h"

namespace Cellnta {

class Snapshot : public Adjustable {
 public:
  using CellList = std::vector<Cell>;

  void SetDimension(int dim);
  int GetDimension() const { return m_dim; }

  class WholeIter : public IterBase::CellForward {
   public:
    WholeIter(const Snapshot* snap);
    void Reset() override;

    const Cell* Next() override;

   private:
    const Snapshot* m_snap;
    Cell m_curr;
    CellList::const_iterator m_iter;
  };

  WholeIter MakeWholeIter() const;

  class AreaIter : public IterBase::CellForward {
   public:
    AreaIter(const Snapshot* snap, const Area& area);
    void Reset() override;

    const Cell* Next() override;

   private:
    const Snapshot* m_snap;
    Cell m_curr;
    Area m_area;
    CellList::const_iterator m_iter;
  };

  AreaIter MakeAreaIter(const Area& area) const;

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos& pos) const override;

 private:

  CellList m_data;
  int m_dim = 0;
};

}  // namespace Cellnta
