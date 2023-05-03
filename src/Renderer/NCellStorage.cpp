#include "Cellnta/Renderer/NCellStorage.h"

#include <cassert>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

void NCellStorage::Restore() {
  CELLNTA_PROFILE;

  m_visibleCells = m_cells;
}

void NCellStorage::Add(const Vec& pos) {
  CELLNTA_PROFILE;

  Vec tmp = Vec(m_d + 1);

  if (pos.size() >= m_d) {
    memcpy(tmp.data(), pos.data(), m_d * sizeof(Point));
  } else {
    const size_t offset = pos.size();
    const size_t rem = tmp.size() - offset - 1;
    memcpy(tmp.data(), pos.data(), offset * sizeof(Point));
    memset(tmp.data() + offset, 0, rem * sizeof(Point));
  }

  tmp(m_d) = 1;

  m_cells.push_back(std::move(tmp));
  m_needUpdate = true;
}

void NCellStorage::AddHomogeneous(const Vec& pos) {
  CELLNTA_PROFILE;

  if (pos.size() != m_d + 1) {
    CELLNTA_LOG_WARN(
        "Unable to set homogeneous pos:"
        "expcected size: {} (dim + 1), but passed: {}",
        m_d + 1, pos.size());
    return;
  }

  m_cells.push_back(pos);
  m_needUpdate = true;
}

void NCellStorage::Erase(const VecList::iterator& pos) {
  CELLNTA_PROFILE;

  m_cells.erase(pos);
  m_needUpdate = true;
}

void NCellStorage::Erase(const Vec& pos) {
  CELLNTA_PROFILE;

  // In future i want to make m_cells as a hash map
  // So its temporary
  auto it = m_cells.begin();
  while (it != m_cells.end()) {
    if (it->block(0, 0, 1, m_d) == pos) {
      m_cells.erase(it);
      break;
    }
    ++it;
  }

  m_needUpdate = true;
}

void NCellStorage::EraseHomogeneous(const Vec& pos) {
  CELLNTA_PROFILE;

  auto it = m_cells.begin();
  while (it != m_cells.end()) {
    if (*it == pos) {
      m_cells.erase(it);
      break;
    }
    ++it;
  }

  m_needUpdate = true;
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

  m_needUpdate = true;
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
