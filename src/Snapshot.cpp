#include "Cellnta/Snapshot.h"
#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

Snapshot::WholeIter::WholeIter(const Snapshot& snap) : m_snap(&snap) {
  Reset();
}

void Snapshot::WholeIter::Reset() {
  m_curr.pos = Cell::Pos::Zero(0);
  m_iter = m_snap->m_data.begin();
}

const Cell* Snapshot::WholeIter::Next() {
  if (m_iter == m_snap->m_data.end())
    return nullptr;

  m_curr = *m_iter;
  ++m_iter;
  return &m_curr;
}

Snapshot::AreaIter::AreaIter(const Snapshot& snap, const Area& area)
    : m_snap(&snap), m_area(area) {
  Reset();
}

void Snapshot::AreaIter::Reset() {
  m_curr.pos = Cell::Pos::Zero(0);
  m_iter = m_snap->m_data.begin();
}

const Cell* Snapshot::AreaIter::Next() {
  while (true) {
    if (m_iter == m_snap->m_data.end())
      return nullptr;
    if (m_area.PosValid(m_iter->pos))
      break;
    ++m_iter;
  }

  m_curr = *m_iter;
  ++m_iter;
  return &m_curr;
}

void Snapshot::SetDimension(int dim) {
  if (dim == m_dim)
    return;

  CELLNTA_LOG_TRACE("Clearing snapshot data, because dimension changed");
  m_data.clear();
  m_dim = dim;
}

bool Snapshot::OnSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  if (cell.pos.size() != m_dim) {
    CELLNTA_LOG_WARN(
        "Not expected cell dimension: actually - {}, expected - {}",
        cell.pos.size(), m_dim);
    return true;
  }

  m_data.push_back(cell);
  return false;
}

Cell::State Snapshot::OnGetCell(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  if (pos.size() != m_dim) {
    CELLNTA_LOG_WARN(
        "Not expected cell dimension: actually - {}, expected - {}", pos.size(),
        m_dim);
    return 0;
  }

  for (const Cell& cell : m_data)
    if (cell.pos == pos)
      return cell.state;
  return 0;
}

Snapshot::WholeIter Snapshot::MakeWholeIter() const {
  return Snapshot::WholeIter(*this);
}

Snapshot::AreaIter Snapshot::MakeAreaIter(const Area& area) const {
  return Snapshot::AreaIter(*this, area);
}
