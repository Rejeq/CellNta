#pragma once

#include <vector>

#include <Eigen/Dense>

#include "Config.h"
#include "GlBackend.h"
#include "Shader.h"
#include "Camera3d.h"
#include "CameraNd.h"
#include "HypercubeStorage.h"
#include "NCellStorage.h"
#include "ColorStorage.h"
#include "RenderData.h"

enum class RenderMode
{
  Projection,
  Collating,
};

class Renderer
{
public:
	Renderer() = default;
	~Renderer();

	void Init(uint32_t D);

	void Update();

	void Draw();
	void DrawGrid();
  bool WantDraw() const { return m_wantDraw; }

	template<bool homogeneous = true, typename Derived>
	void AddCell(const Derived& pos)
  {
    if(m_cells.Add<homogeneous>(pos))
    {
      glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
      glBufferData(GL_ARRAY_BUFFER, m_cells.PointsCapacity3dInBytes(), nullptr, GL_DYNAMIC_DRAW);
    }
    m_updateVboCells = true;
  }

	void GenrateHypercube(float a = -1, CubeMode mode = CubeMode::NONE);
  void Rotate(size_t axis1, size_t axis2, float angle);
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

	size_t GetDimensions() const { return m_d; }
  CubeMode GetCubeMode() const { return m_cube.GetMode(); }

	const Camera3d& GetCamera3d() const { return p_camera; }
	Camera3d& GetCamera3d() { return p_camera; }
	const std::vector<CameraNd>& GetNdCameras() const { return p_Ncameras; }
	std::vector<CameraNd>& GetNdCameras() { return p_Ncameras; }
  const NCellStorage& GetCells() const { return m_cells; }

	size_t GetDimensionsMaxSize();

  const RenderData& GetData() const { return m_renderData; }
  RenderData& GetData() { return m_renderData; }

protected:
	Camera3d p_camera;
	std::vector<CameraNd> p_Ncameras;

private:

  void UpdateOnlyCameraMatrix();
	void UpdateCamera();

  void UpdateCubeBuffer();
	void UpdateCellBuffer();
  void UpdateData();

  void UpdateColorTexture();

	void InitBuffers();

	void BeginArrayBufferSource(float*& dst, size_t offset, size_t size);
	void EndArrayBufferSource();


  RenderData m_renderData;

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

  std::vector<size_t> CollatingValues;

  size_t m_d = 0; // dimensions
	bool m_updateVboCells = false;
  bool m_wantDraw = false;
};

