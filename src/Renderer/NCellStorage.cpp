#include "Cellnta/Renderer/NCellStorage.h"

#include <cassert>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

NCellStorage::NCellStorage() {
  glGenBuffers(1, &m_buffer);
}

NCellStorage::~NCellStorage() {
  if (m_buffer != 0)
    glDeleteBuffers(1, &m_buffer);
}

void NCellStorage::Restore() {
  CELLNTA_PROFILE;

  m_visibleCells = m_cells;
}

void NCellStorage::Add(const Vec& pos) {
  CELLNTA_PROFILE;

  Vec tmp = Vec::Zero(m_d + 1);
  memcpy(tmp.data(), pos.data(), std::min(m_d, (int) pos.size()) * sizeof(Point));
  tmp(m_d) = 1;

  m_cells.push_back(tmp);

  m_needUpdate = true;
}

void NCellStorage::AddHomogeneous(const Vec& pos) {
  CELLNTA_PROFILE;

  Vec tmp = Vec::Zero(m_d + 1);
  memcpy(tmp.data(), pos.data(), std::min(m_d + 1, (int) pos.size()) * sizeof(Point));
  m_cells.push_back(pos);

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


void NCellStorage::UpdateBuffer() {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating cell buffer");
  if (m_visibleCells.empty())
    return;

  int pointsCount = m_visibleCells.size() * 3;
  Eigen::VectorXf* pnt = m_visibleCells.data();

  if (pnt == nullptr)
    return;

  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

  size_t capacity = m_visibleCells.capacity();
  if (capacity != m_oldCapacity) {
    m_oldCapacity = capacity;
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
    glBufferData(GL_ARRAY_BUFFER,
                 3 * capacity * sizeof(NCellStorage::Point), nullptr,
                 GL_DYNAMIC_DRAW);
  }

  float* dst =
      BeginArrayBufferSource(0, pointsCount * sizeof(NCellStorage::Point));

  if (dst == nullptr)
    return;

  const float* end = dst + pointsCount;

  // memset(dst, 0, GetTotalAllocatedMemoryForPoints());
  while (dst < end) {
    dst[0] = (*pnt)(0);
    dst[1] = (*pnt)(1);
    dst[2] = (*pnt)(2);
    ++pnt;
    dst += 3;
  }

  EndArrayBufferSource();
}
