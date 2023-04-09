#pragma once

#include <cassert>
#include <vector>

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

  bool NeedUpdatePoints() const { return m_needUpdatePoints; }
  void PointsHandled() { m_needUpdatePoints = false; }

  bool NeedUpdateIndices() const { return m_needUpdateIndices; }
  void IndicesHandled() { m_needUpdateIndices = false; }

  void Restore();

  void GenerateCube(int dim, Point size, CubeMode mode);
  void SetDimension(int dim);
  void SetSize(Point size);
  void SetMode(CubeMode mode);

  CubeMode GetMode() const { return m_mode; }
  int GetDimensions() const { return m_d; }
  float GetSize() const { return m_cubeSize; }

  inline int GetVerticesCount() const { return GetVerticesCount(m_d); }
  inline int GetVertexSize() const { return m_d + 1; }
  inline int GetFacesCount() const { return GetFacesCount(m_d); }
  inline int GetEdgesCount() const { return GetEdgesCount(m_d); }

  const ColorStorage& GetColor() const { return m_color; }
  ColorStorage& GetColor() { return m_color; }

  int GetPointsSize() const { return GetVerticesCount() * GetVertexSize(); }
  const Point* GetPoints() const { return m_pnt.data(); }
  Point* GetPoints() { return m_pnt.data(); }

  int GetIndicesSize() const { return m_ind.size(); }
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
  int GetPlanesCount(int dim) const;

  int GetRemaindingFacesCount(int dim) const;
  int GetFacesCount(int dim) const;

  int GetEdgesCount(int dim) const;

  int GetVerticesCount(int dim) const;


  std::vector<Point> m_origPnt;
  std::vector<Point> m_pnt;
  std::vector<Ind> m_ind;

  ColorStorage m_color;

  CubeMode m_mode = CubeMode::WIREFRAME;
  float m_cubeSize = 0.5f;
  bool m_needUpdatePoints = false;
  bool m_needUpdateIndices = false;
  int m_d = 0;
};

}  // namespace Cellnta
