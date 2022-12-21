#include "Cellnta/Renderer/NCellStorage.h"

#include <cassert>

#include "Cellnta/Config.h"

using namespace Cellnta;

void NCellStorage::Restore() {
  CELLNTA_PROFILE;

  m_visibleCells = m_cells;
}

void NCellStorage::Add(const Vec& pos) {
  CELLNTA_PROFILE;

  assert(pos.size() >= m_d);

  Vec tmp(m_d + 1);
  memcpy(tmp.data(), pos.data(), m_d * sizeof(Point));
  tmp(m_d) = 1;

  m_cells.push_back(tmp);
}

void NCellStorage::AddHomogeneous(const Vec& pos) {
  CELLNTA_PROFILE;

  assert(pos.size() >= m_d + 1);
  m_cells.push_back(pos);
}

void NCellStorage::SetDimension(const int dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  clear();
}

void NCellStorage::clear() {
  CELLNTA_PROFILE;

  m_visibleCells.clear();
  m_cells.clear();
};

void NCellStorage::reserve(int capacity) {
  CELLNTA_PROFILE;

  m_visibleCells.reserve(capacity);
  m_cells.reserve(capacity);
};

size_t NCellStorage::capacity() const {
  return m_cells.capacity();
}

size_t NCellStorage::size() const {
  return m_cells.size();
}
