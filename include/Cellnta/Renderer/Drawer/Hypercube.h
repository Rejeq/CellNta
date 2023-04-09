#pragma once

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/Drawer/Color.h"

namespace Cellnta {

class HypercubeDrawer {
 public:
  HypercubeDrawer();
  ~HypercubeDrawer();

  void UpdatePoints(const HypercubeStorage& cube);
  void UpdateIndices(HypercubeStorage& cube);
  void UpdateColor(HypercubeStorage& cube);

  GLuint GetPointsBuffer() const { return m_points; }

  GLuint GetIndicesBuffer() const { return m_indices; }
  GLuint GetIndicesSize() const { return m_indicesSize; }

  const ColorDrawer& GetColor() const { return m_color; }
  ColorDrawer& GetColor() { return m_color; }

 private:
  GLuint m_points = 0;
  int m_pointsCapacity = 0;

  GLuint m_indices = 0;
  GLuint m_indicesSize = 0;
  ColorDrawer m_color;
};

}  // namespace Cellnta

