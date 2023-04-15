#include "Cellnta/Renderer/Drawer/Hypercube.h"

using namespace Cellnta;

HypercubeDrawer::HypercubeDrawer() {
  glGenBuffers(1, &m_points);
  glGenBuffers(1, &m_indices);
}

HypercubeDrawer::~HypercubeDrawer() {
  glDeleteBuffers(1, &m_points);
  glDeleteBuffers(1, &m_indices);
}

void HypercubeDrawer::UpdatePoints(const HypercubeStorage& cube) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating points buffer");

  glBindBuffer(GL_ARRAY_BUFFER, m_points);

  const int size = cube.GetPointsSize();
  if (size != m_pointsCapacity) {
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(HypercubeStorage::Point),
                 nullptr, GL_DYNAMIC_DRAW);
    m_pointsCapacity = size;
  }

  const int pointsCount = cube.GetVerticesCount() * 3;
  const float* pnt = cube.GetPoints();

  if (pnt == nullptr)
    return;

  float* dst =
      BeginArrayBufferSource(0, size * sizeof(HypercubeStorage::Point));

  if (dst == nullptr)
    return;

  const float* end = dst + pointsCount;
  while (dst < end) {
    dst[0] = pnt[0];
    dst[1] = pnt[1];
    dst[2] = pnt[2];

    pnt += cube.GetVertexSize();
    dst += 3;
  }

  EndArrayBufferSource();
}

void HypercubeDrawer::UpdateIndices(HypercubeStorage& cube) {
  CELLNTA_PROFILE;

  ChangeMode(cube.GetMode());

  UpdateColor(cube);
  m_indicesSize = cube.GetIndicesSize();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indices);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               m_indicesSize * sizeof(HypercubeStorage::Ind), cube.GetIndices(),
               GL_DYNAMIC_DRAW);
}

void HypercubeDrawer::UpdateColor(HypercubeStorage& cube) {
  auto& color = cube.GetColor();
  switch (cube.GetMode()) {
    case CubeMode::POINTS: color.Generate(cube.GetVerticesCount(), 1); break;
    case CubeMode::WIREFRAME: color.Generate(cube.GetEdgesCount(), 1); break;
    case CubeMode::POLYGON: color.Generate(cube.GetFacesCount(), 2); break;
    default: assert(0 && "Unreachable"); break;
  }
}

bool HypercubeDrawer::ChangeMode(CubeMode mode) {
  GLenum res = 0;

  switch (mode) {
    case CubeMode::POINTS: res = GL_POINTS; break;
    case CubeMode::WIREFRAME: res = GL_LINES; break;
    case CubeMode::POLYGON: res = GL_TRIANGLES; break;
    default: assert(0 && "Unreachable"); break;
  }

  if (res == m_mode)
    return 1;

  m_mode = res;
  return 0;
}
