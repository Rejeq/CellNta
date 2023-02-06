#pragma once

#include "Cellnta/Area.h"
#include "Cellnta/Iterator.h"

namespace Cellnta {

class Adjustable {
 public:
  virtual ~Adjustable() = default;

  bool SetCell(const Cell& cell);
  bool SetCell(const Cell::Pos& pos, const Cell::State& state);
  bool SetCell(const std::vector<Cell>& cells);
  bool SetCell(const IteratorRef& cellsIter);

  Cell::State GetCell(const Cell::Pos& pos) const;

 protected:
  virtual bool OnSetCell(const Cell& cell) = 0;
  virtual Cell::State OnGetCell(const Cell::Pos& pos) const = 0;
};

}  // namespace Cellnta
