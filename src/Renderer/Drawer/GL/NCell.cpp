#include "Cellnta/Renderer/Drawer/GL/NCell.h"

using namespace Cellnta;

NCellDrawerGL::NCellDrawerGL() {
  glGenBuffers(1, &m_buffer);
}

NCellDrawerGL::~NCellDrawerGL() {
  glDeleteBuffers(1, &m_buffer);
}

void NCellDrawerGL::Update(const NCellStorage& cells) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating NCellDrawerGL");

  if (cells.Size() == 0) {
    m_size = 0;
    return;
  }

  int pointsCount = cells.Size() * 3;
  size_t capacity = cells.Capacity();

  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

  if (capacity != m_capacity) {
    m_capacity = capacity;
    glBufferData(GL_ARRAY_BUFFER, 3 * capacity * sizeof(NCellStorage::Point),
                 nullptr, GL_DYNAMIC_DRAW);
  }

  float* dst =
      BeginArrayBufferSource(0, pointsCount * sizeof(NCellStorage::Point));
  if (dst == nullptr)
    return;

  const float* end = dst + pointsCount;

  auto iter = cells.MakeVisibleIter();
  const NCellStorage::Cell* cell = nullptr;

  while (dst < end) {
    cell = iter.Next();
    assert(cell != nullptr && "Cell must be valid");

    dst[0] = cell->pos[0];
    dst[1] = cell->pos[1];
    dst[2] = cell->pos[2];
    dst += 3;
  }
  assert(dst >= end && iter.Next() == nullptr && "Not all cells are moved to video memory");

  EndArrayBufferSource();
  m_size = cells.Size();
}
