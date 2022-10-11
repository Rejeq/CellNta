#pragma once

#include <vector>
#include <cassert>

#include "Config.h"

enum class CubeMode
{
  NONE,
	WIREFRAME,
	POLYGON,
  POINTS,
  COUNT,
};

class HypercubeStorage
{
public:
  using point_t = float;
  using ind_t = uint16_t;

  enum Updated
  {
    NONE = 0,
    VERTICES = 1 << 0,
    INDICES  = 1 << 1,
  };

  uint32_t GenerateCube(size_t dim, point_t a, CubeMode solid);
  void Restore();

  void SetMode(CubeMode mode);

  CubeMode GetMode() const { return m_mode; }
  size_t GetDimensions() const { return m_d; }
  float GetScale() const { return m_cubeSize; }

  inline size_t GetVerticesCount() const { return GetVerticesCount(m_d); }
  inline size_t GetVertexSize() const { return m_d + 1; }
  inline size_t GetFacesCount() const { return GetFacesCount(m_d); }


  size_t GetPointsSize() const { return GetVerticesCount() * GetVertexSize(); }
  size_t GetPointsSizeInBytes() const { return GetPointsSize() * sizeof(point_t); }
  const point_t* GetPoints() const { return m_pnt.data(); }
  point_t* GetPoints() { return m_pnt.data(); }

  size_t GetIndicesSize() const { return m_ind.size(); }
  size_t GetIndicesSizeInBytes() const { return GetIndicesSize() * sizeof(ind_t); }
  const ind_t* GetIndices() const { return m_ind.data(); }
  ind_t* GetIndices() { return m_ind.data(); }

private:
  void GenerateVertices();
  void GenerateIndicesPolygon();
  void GenerateIndicesWireframe();
  void GenerateIndicesPoints();

  //Planes:
  //2d: 1  - xy
  //3d: 3  - xy,xz,yz
  //4d: 6  - xy,xz,yz,xw,yw,zw
  //5d: 10 - xy,xz,yz,xw,yw,zw,xe,ye,ze,we
  inline size_t GetPlanesCount(const size_t& dim) const
  {
    //Optimized binomial coefficient
    //where n=dim, k=2
    return dim * (dim - 1) >> 1;
  }

  inline size_t GetRamaindingFaces(const size_t& dim) const { assert(dim > 1); return ((size_t)1) << (dim - 2); }
  inline size_t GetFacesCount(const size_t& dim) const { return GetPlanesCount(dim) * GetRamaindingFaces(dim); }

  inline size_t GetVerticesCount(size_t dim) const { return ((size_t)1) << dim; }

	CubeMode m_mode = CubeMode::WIREFRAME;

	std::vector<point_t> m_pnt;
	std::vector<ind_t> m_ind;

	std::vector<point_t> m_origPnt;

	size_t m_d = 0;
	float m_cubeSize = 0.5f;
};
