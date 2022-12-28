#pragma once

#include <memory>
#include <vector>

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/RenderData.h"
#include "Cellnta/Renderer/Shader.h"

namespace Cellnta {

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

  void GenrateHypercube(float a = -1, CubeMode mode = CubeMode::NONE);
  void Rotate();
  void ProjectBuffers(bool projectCube = true, bool projectCells = true);

  void SetRenderDistance(uint32_t distance);
  void SetDimension(uint32_t D);
  void SetCubeMode(CubeMode mode);

  int GetDimensions() const { return m_d; }
  CubeMode GetCubeMode() const { return m_cube.GetMode(); }

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

  const RenderData* GetData() const { return &m_data; }
  RenderData* GetData() { return &m_data; }

 private:
  void InitBuffers();

  void UpdateCameraUniform();
  void UpdateRenderDistanceUniform();

  void UpdateCamera();
  void UpdateCamera3d();
  void UpdateCameraNd();


  std::shared_ptr<Camera3d> m_camera3d;
  std::shared_ptr<CameraNdList> m_cameraNd;
  HypercubeStorage m_cube;
  RenderData m_data;

  Shader m_cellShader;
  Shader m_gridShader;

  GLuint m_vao = 0;
  bool m_wantDraw = false;
  int m_d = 0;
};

}  // namespace Cellnta
