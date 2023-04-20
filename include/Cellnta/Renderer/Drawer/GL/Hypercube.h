#pragma once

#include "Cellnta/Renderer/Drawer/GL/Color.h"
#include "Cellnta/Renderer/Drawer/GL/GL.h"
#include "Cellnta/Renderer/Drawer/Hypercube.h"

namespace Cellnta {

class HypercubeDrawerGL : public HypercubeDrawer {
 public:
  HypercubeDrawerGL();
  ~HypercubeDrawerGL();

  void UpdatePoints(const HypercubeStorage& cube) override;
  void UpdateIndices(HypercubeStorage& cube) override;
  void UpdateColor(HypercubeStorage& cube) override;

  GLenum GetMode() const { return m_mode; }

  GLuint GetPointsBuffer() const { return m_points; }

  GLuint GetIndicesBuffer() const { return m_indices; }
  GLuint GetIndicesSize() const { return m_indicesSize; }

  const ColorDrawerGL& GetColor() const override { return m_color; }
  ColorDrawerGL& GetColor() override { return m_color; }

 private:
  bool ChangeMode(CubeMode mode);

  GLenum m_mode = 0;

  GLuint m_points = 0;
  int m_pointsCapacity = 0;

  GLuint m_indices = 0;
  GLuint m_indicesSize = 0;
  ColorDrawerGL m_color;
};

}  // namespace Cellnta
