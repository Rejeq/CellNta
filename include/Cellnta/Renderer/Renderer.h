#pragma once

#include <memory>
#include <vector>

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/Shader.h"
#include "Cellnta/Renderer/Drawer/NCell.h"
#include "Cellnta/Renderer/Drawer/Hypercube.h"

namespace Cellnta {

class HypercubeStorage;
class RenderData;

class CameraController;
class Camera3d;
class CameraNd;
using CameraNdList = std::vector<CameraNd>;

class Renderer {
 public:
  Renderer();
  ~Renderer();

  bool CreateShaders(const std::string& gridPath, const std::string& cellPath);

  void Update();

  void Draw();
  void DrawGrid();
  bool WantDraw() const { return m_wantDraw; }

  void Rotate();
  void ProjectBuffers(bool projectCube = true, bool projectCells = true);

  void SetRenderDistance(uint32_t distance);
  void SetDimension(uint32_t D);

  int GetDimensions() const { return m_d; }

  void SetCamera3d(const std::shared_ptr<Camera3d>& camera) {
    m_camera3d = camera;
  }
  const Camera3d* GetCamera3d() const { return m_camera3d.get(); }
  Camera3d* GetCamera3d() { return m_camera3d.get(); }

  void SetCameraNd(const std::shared_ptr<CameraNdList>& camera) {
    m_cameraNd = camera;
  }
  const CameraNdList* GetCameraNd() const { return m_cameraNd.get(); }
  CameraNdList* GetCameraNd() { return m_cameraNd.get(); }

  void SetHypercube(const std::shared_ptr<HypercubeStorage>& cube);
  const HypercubeStorage* GetHypercube() const { return m_cube.get(); }
  HypercubeStorage* GetHypercube() { return m_cube.get(); }

  void SetData(const std::shared_ptr<RenderData>& data);
  const RenderData* GetData() const { return m_data.get(); }
  RenderData* GetData() { return m_data.get(); }

 private:
  void UpdateCameraUniform();
  void UpdateRenderDistanceUniform();

  void UpdateCamera();
  void UpdateCamera3d();
  void UpdateCameraNd();


  std::shared_ptr<Camera3d> m_camera3d;
  std::shared_ptr<CameraNdList> m_cameraNd;
  std::shared_ptr<HypercubeStorage> m_cube;
  std::shared_ptr<RenderData> m_data;

  NCellDrawer m_cellDrawer;
  HypercubeDrawer m_cubeDrawer;

  Shader m_cellShader;
  Shader m_gridShader;

  GLuint m_vao = 0;
  bool m_wantDraw = false;
  int m_d = 0;
};

}  // namespace Cellnta
