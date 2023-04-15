#pragma once

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/Drawer/Hypercube.h"
#include "Cellnta/Renderer/Drawer/NCell.h"
#include "Cellnta/Renderer/Shader.h"

namespace Cellnta {

class SceneDrawer {
 public:
  SceneDrawer();
  ~SceneDrawer();

  bool CreateShaders(const std::string& gridPath, const std::string& cellPath);

  void DrawGrid();
  void Draw();

  GLuint GetVao() const { return m_vao; }

  void UpdateDistance(float distance);
  void UpdateCamera(const Eigen::Matrix4f& projView);

  HypercubeDrawer& GetCube() { return m_cubeDrawer; }
  const HypercubeDrawer& GetCube() const { return m_cubeDrawer; }

  NCellDrawer& GetCell() { return m_cellsDrawer; }
  const NCellDrawer& GetCell() const { return m_cellsDrawer; }

 private:
  Shader m_cellShader;
  Shader m_gridShader;

  HypercubeDrawer m_cubeDrawer;
  NCellDrawer m_cellsDrawer;

  GLuint m_vao = 0;
};

}  // namespace Cellnta
