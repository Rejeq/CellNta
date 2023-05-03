#include "Cellnta/Cell.h"

using namespace Cellnta;

bool Cell::operator==(const Cell& r) const {
  return (pos == r.pos) && (state == r.state);
}

bool Cell::operator!=(const Cell& r) const {
  return !(*this == r);
}
