#pragma once

#include <memory>
#include <vector>

#include "Cellnta/Renderer/DrawerBackend.h"

namespace Cellnta {

class HypercubeStorage;
class RenderData;

class CameraController;
class Camera3d;
class CameraNd;
using CameraNdList = std::vector<CameraNd>;

class Renderer {
 public:
  bool Init(DrawerBackend backend);

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

  void SetIgnoreCubeProject(bool newState);
  bool GetIgnoreCubeProject() const { return m_ignCubeProj; }

  void SetIgnoreCellProject(bool newState);
  bool GetIgnoreCellProject() const { return m_ignCellProj; }

 private:
  void UpdateCameraUniform();
  void UpdateRenderDistanceUniform();

  void UpdateCamera();
  void UpdateCamera3d();
  void UpdateCameraNd();

  bool NeedForceUpdateCubePoints() const;
  bool NeedForceUpdateCells() const;

  std::shared_ptr<Camera3d> m_camera3d;
  std::shared_ptr<CameraNdList> m_cameraNd;
  std::shared_ptr<HypercubeStorage> m_cube;
  std::shared_ptr<RenderData> m_data;

  std::unique_ptr<SceneDrawer> m_sceneDrawer;

  int m_d = 0;
  bool m_wantDraw = false;
  bool m_ignCubeProj = false;
  bool m_ignCellProj = false;
  bool m_prevIgnCubeProj = false;
  bool m_prevIgnCellProj = false;
};

}  // namespace Cellnta
