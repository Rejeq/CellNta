#include "Cellnta/Renderer/NCellStorage.h"

#include <cassert>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

void NCellStorage::RestoreVisible() {
  CELLNTA_PROFILE;

  m_visible.resize(m_cells.Size() * GetVisibleStride(), 0);

  auto iter = m_cells.MakeWholeIter();
  uint8_t* currVisible = m_visible.data();

  while (const Cell* cell = iter.Next()) {
    Point* posPtr = (Point*)&currVisible[GetVisiblePosOffset()];
    Cell::State* statePtr = (Cell::State*)&currVisible[GetVisibleStateOffset()];

    const int posSize = GetVisiblePosLength() - 1;
    assert(cell->pos.size() == posSize);
    FillShuffled(posPtr, cell->pos.data(), posSize);
    *(posPtr + posSize) = 1.0f;

    *statePtr = cell->state;

    currVisible += GetVisibleStride();
  }
}

void NCellStorage::Add(const Vec& pos) {
  CELLNTA_PROFILE;

  if (pos.size() >= m_d) {
    m_cells.Set(pos.head(m_d), 1);
  } else {
    Vec tmp = Vec(m_d);
    const size_t offset = pos.size();
    const size_t rem = tmp.size() - offset;

    memcpy(tmp.data(), pos.data(), offset * sizeof(Point));
    memset(tmp.data() + offset, 0, rem * sizeof(Point));

    m_cells.Set(tmp, 1);
  }

  m_needUpdate = true;
}

void NCellStorage::Erase(const Vec& pos) {
  CELLNTA_PROFILE;

  if (pos.size() >= m_d) {
    m_cells.Erase(pos.head(m_d));
  } else {
    Vec tmp = Vec(m_d);
    const size_t offset = pos.size();
    const size_t rem = tmp.size() - offset - 1;

    memcpy(tmp.data(), pos.data(), offset * sizeof(Point));
    memset(tmp.data() + offset, 0, rem * sizeof(Point));

    m_cells.Erase(tmp);
  }

  m_needUpdate = true;
}

void NCellStorage::EraseArea(const Area& area) {
  CELLNTA_PROFILE;

  if (m_cells.EraseArea(area))
    m_needUpdate = true;
}

void NCellStorage::SetDimension(int dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  ShuffleAxisStorage::SetDimension(dim);
  Clear();
}

void NCellStorage::ClearVisible() {
  CELLNTA_PROFILE;

  m_visible.clear();
  // Update is not happen here
};

void NCellStorage::Clear() {
  CELLNTA_PROFILE;

  ClearVisible();
  m_cells.Clear();

  m_needUpdate = true;
};

int NCellStorage::Size() const {
  return m_cells.Size();
}

int NCellStorage::Capacity() const {
  return m_cells.Capacity();
}

NCellStorage::Iter NCellStorage::MakeIter() const {
  Cell::Pos pos = Cell::Pos::Zero(m_d);

  return Iter(*this, m_cells.MakeWholeIter(), std::move(pos), pos.size());
}

NCellStorage::Iter NCellStorage::MakeHomogeneousIter() const {
  Cell::Pos pos = Cell::Pos::Zero(m_d + 1);
  pos[m_d] = 1.0f;

  return Iter(*this, m_cells.MakeWholeIter(), std::move(pos), pos.size() - 1);
}

NCellStorage::IterVisible NCellStorage::MakeVisibleIter() const {
  assert(!m_visible.empty() && "Visible cells is not populated");

  assert(m_cells.Size() == GetVisibleSize() &&
         "Visible cells must be a modification of the cells");
  return IterVisible(*this, true);
}

NCellStorage::IterVisible NCellStorage::MakeNoShuffleVisibleIter() const {
  assert(!m_visible.empty() && "Visible cells is not populated");

  assert(m_cells.Size() == GetVisibleSize() &&
         "Visible cells must be a modification of the cells");
  return IterVisible(*this, false);
}

void NCellStorage::Iter::Reset() {
  m_iter.Reset();
}

const NCellStorage::Cell* NCellStorage::Iter::Next() {
  const Cell* tmp = m_iter.Next();
  if (tmp == nullptr)
    return nullptr;

  assert(tmp->pos.size() <= m_activeAxes);
  m_storage->FillShuffled(m_cell.pos.data(), tmp->pos.data(), m_activeAxes);

  m_cell.state = tmp->state;

  return &m_cell;
}

void NCellStorage::IterVisible::Reset() {
  m_cell.pos = Cell::Pos::Zero(m_storage->GetVisibleSize());
  m_idx = 0;
}

const NCellStorage::Cell* NCellStorage::IterVisible::Next() {
  if (m_idx >= (size_t)m_storage->GetVisibleSize())
    return nullptr;

  const uint8_t* visible = m_storage->m_visible.data();
  const uint8_t* currVisible = &visible[m_idx * m_storage->GetVisibleStride()];

  const uint8_t* posPtr = &currVisible[m_storage->GetVisiblePosOffset()];
  const uint8_t* statePtr = &currVisible[m_storage->GetVisibleStateOffset()];

  Eigen::Map<const Vec> actualPos((Point*)posPtr,
                                  m_storage->GetVisiblePosLength());

  // Positions is already shuffled after RestoreVisible()
  // So if shuffling is not needed, just call FillShuffled again
  if (m_needShuffle)
    m_cell.pos = actualPos;
  else
    m_storage->FillShuffled(m_cell.pos.data(), actualPos.data(),
                            actualPos.size());

  Cell::State* actualState = (Cell::State*)statePtr;
  m_cell.state = *actualState;

  m_idx++;
  return &m_cell;
}
