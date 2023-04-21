#include "Cellnta/Adjustable.h"
#include "Cellnta/Config.h"

using namespace Cellnta;

bool Adjustable::SetCell(const Cell& cell) {
  return OnSetCell(cell);
}

bool Adjustable::SetCell(const Cell::Pos& pos, const Cell::State& state) {
  return SetCell(Cell(pos, state));
}

bool Adjustable::SetCell(const std::vector<Cell>& cells) {
  CELLNTA_PROFILE;

  for (const Cell& cell : cells) {
    if (SetCell(cell))
      return true;
  }
  return false;
}

bool Adjustable::SetCell(const IteratorRef& cellsIter) {
  CELLNTA_PROFILE;

  // TODO: Replace IteartorRef witch Iterator everywhere
  return SetCell(*cellsIter);
}

bool Adjustable::SetCell(Iterator& cellsIter) {
  CELLNTA_PROFILE;

  while (const Cell* cell = cellsIter.Next()) {
    if (SetCell(*cell))
      return true;
  }
  return false;
}

Cell::State Adjustable::GetCell(const Cell::Pos& pos) const {
  return OnGetCell(pos);
}
