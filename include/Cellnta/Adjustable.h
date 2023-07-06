#pragma once

#include <vector>

#include "Cellnta/Cell.h"
#include "Cellnta/Iterator.h"

namespace Cellnta {

class Adjustable {
 public:
  virtual ~Adjustable() = default;

  bool SetCell(const Cell& cell);
  bool SetCell(const Cell::Pos& pos, const Cell::State& state);
  bool SetCell(const std::vector<Cell>& cells);
  bool SetCell(IterBase::CellForward& cellsIter);
  bool SetCell(IterBase::CellForward&& cellsIter);

  Cell::State GetCell(const Cell::Pos& pos) const;

 protected:
  virtual bool OnSetCell(const Cell& cell) = 0;
  virtual Cell::State OnGetCell(const Cell::Pos& pos) const = 0;
};

}  // namespace Cellnta
