#include "Cellnta/Snapshot.h"
#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

class Snapshot::Iterator : public Cellnta::Iterator {
 public:
  Iterator(const Snapshot* snap) : m_snap(snap) {
    if (snap == nullptr)
      CELLNTA_LOG_ERROR("Passing a not initialized Snapshot in Iterator");
    m_curr.pos = Cell::Pos::Zero(0);
    m_iter = m_snap->m_data.begin();
  }

  const Cell* Next() override {
    if (m_iter == m_snap->m_data.end())
      return nullptr;

    m_curr = *m_iter;
    ++m_iter;
    return &m_curr;
  }

 private:
  const Snapshot* m_snap;
  Cell m_curr;
  Snapshot::CellList::const_iterator m_iter;
};

class Snapshot::AreaIterator : public Cellnta::Iterator {
 public:
  AreaIterator(const Snapshot* snap, const Area& area)
      : m_snap(snap), m_area(area) {
    if (snap == nullptr)
      CELLNTA_LOG_ERROR("Passing a not initialized Snapshot in Iterator");
    m_curr.pos = Cell::Pos::Zero(0);
    m_iter = m_snap->m_data.begin();
  }

  const Cell* Next() override {
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

 private:
  const Snapshot* m_snap;
  Cell m_curr;
  Area m_area;
  Snapshot::CellList::const_iterator m_iter;
};

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

  for (const Cell& cell : m_data)
    if (cell.pos == pos)
      return cell.state;
  return 0;
}

std::unique_ptr<Cellnta::Iterator> Snapshot::CreateIterator() const {
  return std::make_unique<Snapshot::Iterator>(this);
}

std::unique_ptr<Cellnta::Iterator> Snapshot::CreateIterator(
    const Area& area) const {
  return std::make_unique<Snapshot::AreaIterator>(this, area);
}
