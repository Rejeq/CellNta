#pragma once

#include <cassert>
#include <vector>

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/ColorStorage.h"

namespace Cellnta {

enum class CubeMode {
  NONE,
  WIREFRAME,
  POLYGON,
  POINTS,
  COUNT,
};

class HypercubeStorage {
 public:
  using Point = float;
  using Ind = uint16_t;

  HypercubeStorage();
  ~HypercubeStorage();

  // TODO: Different usage context with NCellStoarge
  // In NCellStorage updated when within added cell
  // In HypercubeStorage updated when within changed gl buffer
  //
  // True only when some of the OpenGL buffers are changed
  bool NeedUpdate() const { return m_needUpdate; }
  void Handled() { m_needUpdate = false; }

  void GenerateCube(int dim, Point size, CubeMode mode);
  void Restore();

  void UpdatePointsBuffer();
  void UpdateIndicesBuffer();
  void UpdateColor();

  void SetMode(CubeMode mode);

  CubeMode GetMode() const { return m_mode; }
  int GetDimensions() const { return m_d; }
  float GetSize() const { return m_cubeSize; }

  inline int GetVerticesCount() const { return GetVerticesCount(m_d); }
  inline int GetVertexSize() const { return m_d + 1; }
  inline int GetFacesCount() const { return GetFacesCount(m_d); }

  const ColorStorage& GetColor() const { return m_color; }
  ColorStorage& GetColor() { return m_color; }

  int GetPointsSize() const { return GetVerticesCount() * GetVertexSize(); }
  GLuint GetPointsBuffer() const { return m_pointsBuffer; }
  const Point* GetPoints() const { return m_pnt.data(); }
  Point* GetPoints() { return m_pnt.data(); }

  int GetIndicesSize() const { return m_ind.size(); }
  GLuint GetIndicesBuffer() const { return m_indicesBuffer; }
  const Ind* GetIndices() const { return m_ind.data(); }
  Ind* GetIndices() { return m_ind.data(); }

 private:
  void GenerateVertices();
  void GenerateIndicesPolygon();
  void GenerateIndicesWireframe();
  void GenerateIndicesPoints();

  // Planes:
  // 2d: 1  - xy
  // 3d: 3  - xy,xz,yz
  // 4d: 6  - xy,xz,yz,xw,yw,zw
  // 5d: 10 - xy,xz,yz,xw,yw,zw,xe,ye,ze,we
  inline int GetPlanesCount(const int& dim) const {
    // Optimized binomial coefficient
    // where n=dim, k=2
    return dim * (dim - 1) >> 1;
  }

  inline int GetRamaindingFaces(const int& dim) const {
    assert(dim > 1);
    return ((int)1) << (dim - 2);
  }
  inline int GetFacesCount(const int& dim) const {
    return GetPlanesCount(dim) * GetRamaindingFaces(dim);
  }

  inline int GetVerticesCount(int dim) const { return ((int)1) << dim; }


  std::vector<Point> m_origPnt;
  std::vector<Point> m_pnt;
  std::vector<Ind> m_ind;

  ColorStorage m_color;

  GLuint m_pointsBuffer = 0;
  GLuint m_indicesBuffer = 0;
  int m_pointsBufferSize = -1;

  CubeMode m_mode = CubeMode::WIREFRAME;
  float m_cubeSize = 0.5f;
  bool m_needUpdate = false;
  int m_d = 0;
};

}  // namespace Cellnta
