#pragma once

#include "Cellnta/Adjustable.h"
#include "Cellnta/Area.h"
#include "Cellnta/Iterator.h"

namespace Cellnta {

class Snapshot : public Adjustable, public Iterable {
 public:
  void SetDimension(int dim);
  int GetDimension() const { return m_dim; }

  std::unique_ptr<Iterator> CreateIterator() const override;
  std::unique_ptr<Iterator> CreateIterator(const Area& area) const override;

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos& pos) const override;

 private:
  using CellList = std::vector<Cell>;

  class Iterator;
  class AreaIterator;

  CellList m_data;
  int m_dim;
};

}  // namespace Cellnta
