#include "Cellnta/Renderer/HypercubeStorage.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

#define HYPERCUBE_DEBUG_PRINT

#ifdef HYPERCUBE_DEBUG_PRINT
#define HYPERCUBE_VALIDATE_POLYGONS(data_, dim_, polygonsCount_)               \
  {                                                                            \
    assert((data_ < (m_ind.data() + m_ind.size())) && "Out of range");         \
    for (int p_ = 0; p_ < polygonsCount_ * 3; p_ += 3) {                       \
      assert(data_[p_ + 0] < GetVerticesCount(dim_) && "Too large index");     \
      assert(data_[p_ + 1] < GetVerticesCount(dim_) && "Too large index");     \
      assert(data_[p_ + 2] < GetVerticesCount(dim_) && "Too large index");     \
      /*Check a polygon is not line*/                                          \
      assert(data_[p_ + 0] != data_[p_ + 1] && "Same indexes in one polygon"); \
      assert(data_[p_ + 0] != data_[p_ + 2] && "Same indexes in one polygon"); \
      assert(data_[p_ + 1] != data_[p_ + 2] && "Same indexes in one polygon"); \
    }                                                                          \
  }
#else
#define HYPERCUBE_VALIDATE_POLYGONS(data, dim, polygonsCount)
#endif  // HYPERCUBE_DEBUG_PRINT

using namespace Cellnta;

HypercubeStorage::HypercubeStorage() {
  glGenBuffers(1, &m_pointsBuffer);
  glGenBuffers(1, &m_indicesBuffer);
}

HypercubeStorage::~HypercubeStorage() {
  if (m_pointsBuffer != 0)
    glDeleteBuffers(1, &m_pointsBuffer);

  if (m_indicesBuffer != 0)
    glDeleteBuffers(1, &m_indicesBuffer);
}

void HypercubeStorage::Restore() {
  CELLNTA_PROFILE;

  m_pnt = m_origPnt;
}

void HypercubeStorage::GenerateCube(int dim, Point size, CubeMode mode) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_DEBUG("Trying to generate hypercube");

  if (dim != m_d) {
    if (size == (Point)-1)
      size = GetSize();
    if (mode == CubeMode::NONE)
      mode = GetMode();
  } else {
    if (size == GetSize())
      size = -1;
    if (mode == GetMode())
      mode = CubeMode::NONE;
  }

  m_d = dim;

  if (size != -1) {
    SetSize(size);
  }

  SetMode(mode);
}

void HypercubeStorage::SetDimension(int dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  SetSize(GetSize());
  SetMode(GetMode());
}

void HypercubeStorage::SetSize(Point size) {
  CELLNTA_PROFILE;

  if (size < 0)
    return;

  m_cubeSize = size;
  GenerateVertices();

  UpdatePointsBuffer();
}

void HypercubeStorage::SetMode(CubeMode mode) {
  CELLNTA_PROFILE;

  if (mode == CubeMode::NONE)
    return;

  if (m_d == 1 && mode == CubeMode::POLYGON)
    mode = CubeMode::WIREFRAME;
  if (m_d == 0 && (mode == CubeMode::POLYGON || mode == CubeMode::WIREFRAME))
    mode = CubeMode::POINTS;

  m_mode = mode;
  switch (mode) {
    case CubeMode::POLYGON: GenerateIndicesPolygon(); break;
    case CubeMode::WIREFRAME: GenerateIndicesWireframe(); break;
    case CubeMode::POINTS: GenerateIndicesPoints(); break;
    default: break;
  }

  UpdateIndicesBuffer();
}

void HypercubeStorage::GenerateVertices() {
  CELLNTA_PROFILE;

  m_pnt.resize(GetPointsSize());

  const uint32_t halfVert = GetVerticesCount() >> 1;
  Point* data = m_pnt.data();
  Point* sec_data = m_pnt.data() + halfVert * GetVertexSize();
  // TODO: Better use dynamic bitset
  for (uint32_t bitset = 0; bitset < halfVert; ++bitset) {
    uint32_t j = 0;
    uint64_t pos = bitset * GetVertexSize();
    for (; j < (uint32_t)m_d - 1; ++j, ++pos) {
      // Check j bit
      if (bitset & ((uint32_t)1 << j))
        data[pos] = sec_data[pos] = +m_cubeSize;
      else
        data[pos] = sec_data[pos] = -m_cubeSize;
    }
    data[pos] = -m_cubeSize;
    data[pos + 1] = 1.0f;
    sec_data[pos] = +m_cubeSize;
    sec_data[pos + 1] = 1.0f;
  }

  m_pnt.shrink_to_fit();
  m_origPnt = m_pnt;

#ifdef HYPERCUBE_DEBUG_PRINT
  CELLNTA_LOG_DEBUG("Points:");
  for (size_t i = 0; i < m_pnt.size() / GetVertexSize(); ++i) {
    int offset = i * GetVertexSize();
    auto start = m_pnt.begin() + offset;
    auto end = start + GetVertexSize();
    CELLNTA_LOG_DEBUG("{}: {:+f}", i, fmt::join(start, end, ", "));
  }
#endif  // HYPERCUBE_DEBUG_PRINT
}

void HypercubeStorage::GenerateIndicesPolygon() {
  CELLNTA_PROFILE;

  // m_ind.clear();
  m_ind.resize(GetFacesCount() * 6, 0);

  const int usedTotalSize = GetVerticesCount();
  std::unique_ptr<bool[]> usedTotal = std::make_unique<bool[]>(usedTotalSize);
  memset(usedTotal.get(), 0, usedTotalSize * sizeof(bool));

  Ind* data = m_ind.data();

  const int maxVert = GetVerticesCount();

  for (int offset = 0; offset < maxVert;
       offset = (offset == 0) ? 1 : offset << 1) {
    for (int corner = 1; corner < maxVert; corner <<= 1) {
      if (corner == offset)
        continue;

      for (int edge = corner << 1; edge < maxVert; edge <<= 1) {
        if (edge == corner || edge == offset)
          continue;

        int total = edge + corner + offset;
        data[0] = offset;
        data[3] = offset;
        data[1] = corner + offset;
        data[4] = edge + offset;
        data[2] = total;
        data[5] = total;

        HYPERCUBE_VALIDATE_POLYGONS(data, m_d, 2);
        data += 6;

        if (!usedTotal[total]) {
          usedTotal[total] = true;

          for (int conCorner = 1; conCorner < maxVert; conCorner <<= 1) {
            if (conCorner == corner || conCorner == edge || conCorner == offset)
              continue;

            for (int conEdge = conCorner << 1; conEdge < maxVert;
                 conEdge <<= 1) {
              if (conEdge == conCorner || conEdge == corner ||
                  conEdge == edge || conEdge == offset)
                continue;

              int conTotal = conEdge + conCorner + total;
              data[0] = total;
              data[3] = total;
              data[1] = conCorner + total;
              data[4] = conEdge + total;
              data[2] = conTotal;
              data[5] = conTotal;

              HYPERCUBE_VALIDATE_POLYGONS(data, m_d, 2);
              data += 6;
            }
          }
        }
      }
    }
  }

  m_ind.shrink_to_fit();

#ifdef HYPERCUBE_DEBUG_PRINT
  CELLNTA_LOG_DEBUG("Polygon: (faces={})", GetFacesCount());
  for (size_t i = 0; i < m_ind.size(); i += 6) {
    CELLNTA_LOG_DEBUG("Face: {}", i / 6);
    for (int j = 0; j < 2; ++j) {
      constexpr int offset = 3;
      auto start = m_ind.begin() + i + j * offset;
      auto end = start + offset;
      CELLNTA_LOG_DEBUG("{}", fmt::join(start, end, ", "));
    }
  }
#endif  // HYPERCUBE_DEBUG_PRINT
}

void HypercubeStorage::GenerateIndicesWireframe() {
  CELLNTA_PROFILE;

  // m_ind.clear();
  m_ind.resize(GetVerticesCount() * m_d);
  // or m_ind.resize(GetFacesCount() * 4);

  Ind* data = m_ind.data();

  // https://stackoverflow.com/questions/51877294/connecting-points-in-n-dimensional-hyper-cube
  for (int currDim = 0; currDim < m_d; currDim++) {
    const int nVert = ((int)1) << currDim;
    const Ind* indicesEnd = data;

    // copy + shift previous edges
    Ind* old_data = m_ind.data();
    while (old_data < indicesEnd) {
      assert(data < m_ind.data() + m_ind.size() && "Out of range");
      assert(old_data < m_ind.data() + m_ind.size() && "Out of range");

      data[0] = old_data[0] + nVert;
      data[1] = old_data[1] + nVert;
      data += 2;
      old_data += 2;
    }

    // create new edges to join cube copies
    for (int i = 0; i < nVert; ++i) {
      assert(data < m_ind.data() + m_ind.size() && "Out of range");

      data[0] = i;
      data[1] = nVert + i;
      data += 2;
    }
  }

  m_ind.shrink_to_fit();

#ifdef HYPERCUBE_DEBUG_PRINT
  CELLNTA_LOG_DEBUG("Wire frame:");
  for (size_t i = 0; i < m_ind.size(); i += 2)
    CELLNTA_LOG_DEBUG("{}, {}", m_ind[i + 0], m_ind[i + 1]);
#endif
}

void HypercubeStorage::GenerateIndicesPoints() {
  m_ind.resize(GetVerticesCount());
  for (int i = 0; i < GetVerticesCount(); ++i)
    m_ind[i] = i;
}

void HypercubeStorage::UpdatePointsBuffer() {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating points buffer");

  glBindBuffer(GL_ARRAY_BUFFER, m_pointsBuffer);

  const int size = GetPointsSize();
  if (size != m_pointsBufferSize) {
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(Point), nullptr,
                 GL_DYNAMIC_DRAW);
    m_pointsBufferSize = size;
  }

  const int pointsCount = GetVerticesCount() * 3;
  float* pnt = GetPoints();

  if (pnt == nullptr)
    return;

  float* dst = BeginArrayBufferSource(0, GetPointsSize() * sizeof(Point));

  if (dst == nullptr)
    return;

  const float* end = dst + pointsCount;
  while (dst < end) {
    dst[0] = pnt[0];
    dst[1] = pnt[1];
    dst[2] = pnt[2];

    pnt += GetVertexSize();
    dst += 3;
  }

  EndArrayBufferSource();
  m_needUpdate = true;
}

void HypercubeStorage::UpdateIndicesBuffer() {
  CELLNTA_PROFILE;

  UpdateColor();

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetIndicesSize() * sizeof(Ind),
               GetIndices(), GL_DYNAMIC_DRAW);
  m_needUpdate = true;
}

void HypercubeStorage::UpdateColor() {
  switch (GetMode()) {
    case CubeMode::POINTS: m_color.Generate(GetVerticesCount(), 1); break;
    case CubeMode::WIREFRAME: m_color.Generate(GetEdgesCount(), 1); break;
    case CubeMode::POLYGON: m_color.Generate(GetFacesCount(), 2); break;
    default: assert(0 && "Unreachable"); break;
  }
  m_needUpdate = true;
}

int HypercubeStorage::GetEdgesCount(int dim) const {
  // https://oeis.org/A001787
  if (dim < 0)
    return 0;
  return dim * GetVerticesCount(dim - 1);
}

#undef HYPERCUBE_DEBUG_PRINT
#undef HYPERCUBE_VALIDATE_FACE
