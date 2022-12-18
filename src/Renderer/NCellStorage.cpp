#include "Cellnta/Renderer/NCellStorage.h"

#include <cassert>

#include "Cellnta/Config.h"

using namespace Cellnta;

void NCellStorage::Restore() {
  CELLNTA_PROFILE;
  m_cells = m_origCells;
}

void NCellStorage::SetDimension(const int dim) {
  m_d = dim;
  clear();
}

void NCellStorage::clear() {
  m_cells.clear();
  m_origCells.clear();
};

void NCellStorage::reserve(int capacity) {
  m_cells.reserve(capacity);
  m_origCells.reserve(capacity);
};

size_t NCellStorage::capacity() const {
  return m_origCells.capacity();
}

size_t NCellStorage::size() const {
  return m_origCells.size();
}
