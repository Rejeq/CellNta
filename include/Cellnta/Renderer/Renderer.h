#pragma once

#include <memory>
#include <vector>

#include "Cellnta/Renderer/ColorStorage.h"
#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/NCellStorage.h"
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

  template <bool homogeneous = true, typename Derived>
  void AddCell(const Derived& pos) {
    if (m_cells.Add<homogeneous>(pos)) {
      glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
      glBufferData(GL_ARRAY_BUFFER, m_cells.PointsCapacity3dInBytes(), nullptr,
                   GL_DYNAMIC_DRAW);
    }
    m_updateVboCells = true;
  }

  void GenrateHypercube(float a = -1, CubeMode mode = CubeMode::NONE);
  void Rotate();
  void ProjectBuffers();

  int GetCollatingX() const { return m_renderData.GetCollatingX(); }
  int GetCollatingY() const { return m_renderData.GetCollatingY(); }
  int GetCollatingZ() const { return m_renderData.GetCollatingZ(); }

  void SetCollatingX(int x);
  void SetCollatingY(int y);
  void SetCollatingZ(int z);

  void SetRenderDistance(uint32_t distance);
  void SetDimension(uint32_t D);
  void SetCubeMode(CubeMode mode);

  int GetDimensions() const { return m_d; }
  CubeMode GetCubeMode() const { return m_cube.GetMode(); }

  void SetCamera3d(const std::shared_ptr<Camera3d>& camera) { m_camera3d = camera; }
  const Camera3d* GetCamera3d() const { return m_camera3d.get(); }
  Camera3d* GetCamera3d() { return m_camera3d.get(); }

  void SetCameraNd(const std::shared_ptr<CameraNdList>& camera) {
    m_cameraNd = camera;
  }

  const CameraNdList* GetCameraNd() const { return m_cameraNd.get(); }
  CameraNdList* GetCameraNd() { return m_cameraNd.get(); }

  const NCellStorage& GetCells() const { return m_cells; }

  const RenderData& GetData() const { return m_renderData; }
  RenderData& GetData() { return m_renderData; }

 private:
  void InitBuffers();

  void UpdateCameraUniform();
  void UpdateRenderDistanceUniform();

  void UpdateCamera();
  void UpdateCamera3d();
  void UpdateCameraNd();

  void UpdateCubeBuffer();
  void UpdateCellBuffer();
  void UpdateData();

  void UpdateColorTexture();

  static void BeginArrayBufferSource(float*& dst, int offset, int size);
  static void EndArrayBufferSource();

  RenderData m_renderData;

  std::shared_ptr<Camera3d> m_camera3d;
  std::shared_ptr<CameraNdList> m_cameraNd;


  HypercubeStorage m_cube;
  NCellStorage m_cells;
  ColorStorage m_color;

  GLuint m_vao = 0;
  GLuint m_cubeBuffer = 0;
  GLuint m_indicesBuffer = 0;
  GLuint m_cellBuffer = 0;
  GLuint m_colorBuffer = 0;
  GLuint m_colorTexture = 0;

  Shader m_cellShader;
  Shader m_gridShader;

  std::vector<int> CollatingValues;

  int m_d = 0;  // dimensions
  bool m_updateVboCells = false;
  bool m_wantDraw = false;
};

}  // namespace Cellnta
