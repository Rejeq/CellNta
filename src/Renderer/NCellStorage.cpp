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

  m_cells.Set(tmp, 1);
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

  m_cells.Set(pos, 1);
  m_needUpdate = true;
}

void NCellStorage::Erase(const Vec& pos) {
  CELLNTA_PROFILE;

  // TODO: Better make iterator that always return homogenous vector
  // Its better because:
  // - No need store extra axis in every position
  // - No need to make similar methods Erase() and EraseHomogeneous()
  // - No need reallocations
  // - Zero overhead

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

  m_cells.Erase(tmp);
  m_needUpdate = true;
}

void NCellStorage::EraseArea(const Area& area) {
  CELLNTA_PROFILE;

  m_cells.EraseArea(area);
}

void NCellStorage::EraseHomogeneous(const Vec& pos) {
  CELLNTA_PROFILE;

  m_cells.Erase(pos);
  m_needUpdate = true;
}

void NCellStorage::SetDimension(const int dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  Clear();
}

void NCellStorage::Clear() {
  CELLNTA_PROFILE;

  m_visibleCells.Clear();
  m_cells.Clear();

  m_needUpdate = true;
};

int NCellStorage::Size() const {
  return m_cells.Size();
}
