#include "Cellnta/Renderer/Drawer/NCell.h"

using namespace Cellnta;

NCellDrawer::NCellDrawer() {
  glGenBuffers(1, &m_buffer);
}

NCellDrawer::~NCellDrawer() {
  glDeleteBuffers(1, &m_buffer);
}

void NCellDrawer::Update(const NCellStorage& cells) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating NCellDrawer");

  const NCellStorage::VecList& rawCells = cells.GetVisibleRaw();
  if (rawCells.empty())
    return;

  const Eigen::VectorXf* pnt = rawCells.data();
  int pointsCount = rawCells.size() * 3;

  glBindBuffer(GL_ARRAY_BUFFER, m_buffer);

  size_t capacity = rawCells.capacity();
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

  // memset(dst, 0, GetTotalAllocatedMemoryForPoints());
  while (dst < end) {
    dst[0] = (*pnt)(0);
    dst[1] = (*pnt)(1);
    dst[2] = (*pnt)(2);
    ++pnt;
    dst += 3;
  }

  EndArrayBufferSource();
  m_size = rawCells.size();

}
