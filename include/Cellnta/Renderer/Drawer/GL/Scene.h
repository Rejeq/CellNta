#pragma once

#include "Cellnta/Renderer/Drawer/GL/GL.h"
#include "Cellnta/Renderer/Drawer/GL/Hypercube.h"
#include "Cellnta/Renderer/Drawer/GL/NCell.h"
#include "Cellnta/Renderer/Drawer/GL/Shader.h"
#include "Cellnta/Renderer/Drawer/Scene.h"

namespace Cellnta {

class SceneDrawerGL : public SceneDrawer {
 public:
  SceneDrawerGL();
  ~SceneDrawerGL();

  bool CreateShaders(const std::string& gridPath,
                     const std::string& cellPath) override;

  void DrawGrid() override;
  void Draw() override;

  void UpdateDistance(float distance) override;
  void UpdateCamera(const Eigen::Matrix4f& projView) override;

  GLuint GetVao() const { return m_vao; }

  HypercubeDrawer& GetCube() override { return m_cubeDrawer; }
  const HypercubeDrawer& GetCube() const override { return m_cubeDrawer; }

  NCellDrawer& GetCell() override { return m_cellsDrawer; }
  const NCellDrawer& GetCell() const override { return m_cellsDrawer; }

 private:
  Shader m_cellShader;
  Shader m_gridShader;

  HypercubeDrawerGL m_cubeDrawer;
  NCellDrawerGL m_cellsDrawer;

  GLuint m_vao = 0;
};

}  // namespace Cellnta
