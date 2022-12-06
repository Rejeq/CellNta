#include "Cellnta/Renderer/Renderer.h"

#include <type_traits>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/Renderer/Transform.h"

using namespace Cellnta;

Renderer::Renderer() {
  CELLNTA_PROFILE;

  InitBuffers();
  m_color.SetSeed(std::rand() /*123456.789f*/);
}

Renderer::~Renderer() {
  CELLNTA_PROFILE;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  if (m_cubeBuffer != 0)
    glDeleteBuffers(1, &m_cubeBuffer);
  if (m_cellBuffer != 0)
    glDeleteBuffers(1, &m_cellBuffer);
  if (m_indicesBuffer != 0)
    glDeleteBuffers(1, &m_indicesBuffer);
  if (m_colorBuffer != 0)
    glDeleteBuffers(1, &m_colorBuffer);
  if (m_colorTexture != 0)
    glDeleteTextures(1, &m_colorTexture);
  if (m_vao != 0)
    glDeleteVertexArrays(1, &m_vao);
}

bool Renderer::CreateShaders(const std::string& gridPath,
                             const std::string& cellPath) {
  CELLNTA_PROFILE;

  if (m_cellShader.GetID() != 0)
    m_cellShader.Delete();

  if (m_gridShader.GetID() != 0)
    m_gridShader.Delete();

  if (m_gridShader.Create(gridPath)) {
    CELLNTA_LOG_ERROR("Unable to load grid shader");
    return true;
  }

  if (m_cellShader.Create(cellPath)) {
    CELLNTA_LOG_ERROR("Unable to load cell shader");
    return true;
  }

  const Eigen::Vector4f gridColor(0.2f, 0.2f, 0.2f, 0.8f);
  const Eigen::Vector4f xAxisColor(0.1f, 0.1f, 1.0f, 1.0f);
  const Eigen::Vector4f zAxisColor(1.0f, 0.1f, 0.1f, 1.0f);

  m_gridShader.Use();
  m_gridShader.Set("u_gridColor", gridColor);
  m_gridShader.Set("u_xAxisColor", xAxisColor);
  m_gridShader.Set("u_zAxisColor", zAxisColor);

  m_cellShader.Use();
  m_cellShader.Set("u_facesColorTex", 0);

  UpdateRenderDistanceUniform();
  return false;
}

void Renderer::Update() {
  CELLNTA_PROFILE;

  UpdateCamera();

  if (m_renderData.NeedUpdate()) {
    UpdateData();
    m_renderData.Handled();
  }

  if (m_updateVboCells) {
    UpdateCellBuffer();
    m_updateVboCells = false;
  }
}

void Renderer::DrawGrid() {
  CELLNTA_PROFILE;

  m_gridShader.Use();
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

void Renderer::Draw() {
  CELLNTA_PROFILE;

  if (m_cells.size() == 0) {
    m_wantDraw = false;
    return;
  }

  m_cellShader.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_BUFFER, m_colorTexture);

  GLenum renderMode = 0;
  switch (m_cube.GetMode()) {
    case CubeMode::POINTS: renderMode = GL_POINTS;
#if defined(CELLNTA_RENDERER_GL)
      glPointSize(10.0f);
#else
#error "Add support for glPointSize"
#endif
      break;
    case CubeMode::WIREFRAME: renderMode = GL_LINES; break;
    case CubeMode::POLYGON: renderMode = GL_TRIANGLES; break;
    default: assert(0 && "Unreachable"); break;
  }

  static_assert(std::is_same<HypercubeStorage::ind_t, uint16_t>::value,
                "Indices in glDrawElement has another type");
  glDrawElementsInstanced(renderMode, m_cube.GetIndicesSize(),
                          GL_UNSIGNED_SHORT, nullptr, m_cells.size());

  m_wantDraw = false;
}

void Renderer::GenrateHypercube(float a, CubeMode mode) {
  CELLNTA_PROFILE;

  uint32_t updated = m_cube.GenerateCube(m_d, a, mode);

  if (updated & HypercubeStorage::Updated::VERTICES) {
    glBindBuffer(GL_ARRAY_BUFFER, m_cubeBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_cube.GetPointsSizeInBytes(), nullptr,
                 GL_DYNAMIC_DRAW);
    UpdateCubeBuffer();
  }

  if (updated & HypercubeStorage::Updated::INDICES) {
    UpdateColorTexture();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indicesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_cube.GetIndicesSizeInBytes(),
                 m_cube.GetIndices(), GL_DYNAMIC_DRAW);
  }
}

void Renderer::Rotate(int axis1, int axis2, float angle) {
  p_Ncameras[p_Ncameras.size() - 1].Rotate();
  // Eigen::MatrixXf mat;
  // Eigen::Rotation2D rot(angle);

  // const int dim = p_Ncameras.end()->GetDimensions();
  // NCamera& cam = *p_Ncameras.end();
  // cam. = NRotate(dim, 0, dim - 1, radians(5.0f));
}

void Renderer::SetDimension(uint32_t dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  GenrateHypercube(m_cube.GetScale());

  m_renderData.SetDimensions(dim);

  m_cells.SetDimensions(dim);
  p_Ncameras.clear();

  for (uint32_t i = dim; i > 3; --i) {
    Eigen::VectorXf pos = Eigen::VectorXf::Zero(i);
    Eigen::VectorXf front = Eigen::VectorXf::Zero(i);
    CameraNd cam;

    pos(pos.rows() - 1) = 2.0f;
    cam.SetDimensions(i);
    cam.SetPosition(pos);
    cam.SetFront(front);

    p_Ncameras.push_back(cam);
  }
}

void Renderer::SetCubeMode(CubeMode mode) { GenrateHypercube(-1, mode); }

void Renderer::SetCollatingX(int x) { m_renderData.SetCollatingX(x); }

void Renderer::SetCollatingY(int y) { m_renderData.SetCollatingY(y); }

void Renderer::SetCollatingZ(int z) { m_renderData.SetCollatingZ(z); }

void Renderer::SetRenderDistance(uint32_t distance) {
  CELLNTA_PROFILE;

  m_renderData.SetDistance(distance);
  UpdateRenderDistanceUniform();
}

void Renderer::ProjectBuffers() {
  CELLNTA_PROFILE;

  m_cube.Restore();
  m_cells.Restore();

  for (auto& camera : p_Ncameras) {
    if (camera.WantSkip())
      continue;

    const Eigen::MatrixXf viewProj = camera.CalculateViewProj();
    const int dim = camera.GetDimensions();
    const bool usePerspective = camera.GetUsePerspective();

    NProject(m_cube, dim, viewProj, usePerspective);
    NProject(m_cells, dim, viewProj, usePerspective);
  }

  UpdateCubeBuffer();
  UpdateCellBuffer();
}

void Renderer::InitBuffers() {
  CELLNTA_PROFILE;

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_cubeBuffer);
  glGenBuffers(1, &m_cellBuffer);
  glGenBuffers(1, &m_indicesBuffer);
  glGenBuffers(1, &m_colorBuffer);
  glGenTextures(1, &m_colorTexture);

  glBindBuffer(GL_TEXTURE_BUFFER, m_colorBuffer);
  glBindTexture(GL_TEXTURE_BUFFER, m_colorTexture);
  static_assert(std::is_same<ColorStorage::color_t, float>::value,
                "glTexBuffer has another type");
  static_assert(ColorStorage::SIZE == 4, "glTexBuffer has another size");
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_colorBuffer);

  glBindVertexArray(m_vao);

  static_assert(std::is_same<HypercubeStorage::point_t, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeBuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(HypercubeStorage::point_t), nullptr);
  glEnableVertexAttribArray(0);

  static_assert(std::is_same<NCellStorage::point_t, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(NCellStorage::point_t), nullptr);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);
}

void Renderer::BeginArrayBufferSource(float*& dst, int offset, int size) {
  CELLNTA_PROFILE;

  dst =
      (float*)glMapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_WRITE_BIT);
  if (dst == nullptr)
    throw std::runtime_error("cannot get VBO");
}

void Renderer::EndArrayBufferSource() {
  CELLNTA_PROFILE;

  glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Renderer::UpdateCameraUniform() {
  CELLNTA_PROFILE;

  const Eigen::Matrix4f& proj = p_camera.GetProjection();
  const Eigen::Matrix4f& view = p_camera.GetView();
  const Eigen::Matrix4f projView = proj * view;

  m_cellShader.Use();
  m_cellShader.Set("u_projView", projView);

  m_gridShader.Use();
  m_gridShader.Set("u_projView", projView);

  m_wantDraw = true;
}

void Renderer::UpdateRenderDistanceUniform() {
  CELLNTA_PROFILE;

  constexpr float FarPlane = 6.25f * CHUNK_SIZE;

  m_gridShader.Use();
  m_gridShader.Set("u_near", 0.01f);
  m_gridShader.Set("u_far", (float)m_renderData.GetDistance() * FarPlane);
}

void Renderer::UpdateCamera() {
  CELLNTA_PROFILE;

  if (p_camera.Update()) {
    m_renderData.SetPosition(
        Chunk::GetPosFromGlobalPos(p_camera.GetPosition()));
    UpdateCameraUniform();
  }

  bool camUpdated = false;
  for (auto& ncam : p_Ncameras)
    if (ncam.Update())
      camUpdated = true;
  if (camUpdated)
    ProjectBuffers();
}

void Renderer::UpdateCubeBuffer() {
  CELLNTA_PROFILE;

  float* dst = nullptr;
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeBuffer);
  BeginArrayBufferSource(dst, 0, m_cube.GetPointsSizeInBytes());

  const int pointsCount = m_cube.GetVerticesCount() * 3;
  const float* end = dst + pointsCount;
  float* pnt = m_cube.GetPoints();

  while (dst < end) {
    dst[0] = pnt[GetCollatingX()];  // x
    dst[1] = pnt[GetCollatingY()];  // y
    dst[2] = pnt[GetCollatingZ()];  // z

    pnt += m_cube.GetVertexSize();
    dst += 3;
  }
  EndArrayBufferSource();
  m_wantDraw = true;
}

void Renderer::UpdateCellBuffer() {
  CELLNTA_PROFILE;

  if (m_cells.size() == 0)
    return;

  float* dst = nullptr;
  const int pointCount = m_cells.size();

  glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
  BeginArrayBufferSource(dst, 0,
                         pointCount * 3 * sizeof(NCellStorage::point_t));

  const float* end = dst + pointCount * 3;
  Eigen::VectorXf* pnt = m_cells.data();

  // memset(dst, 0, GetTotalAllocatedMemoryForPoints());
  while (dst < end) {
    dst[0] = (*pnt)(GetCollatingX());
    dst[1] = (*pnt)(GetCollatingY());
    dst[2] = (*pnt)(GetCollatingZ());
    ++pnt;
    dst += 3;
  }

  EndArrayBufferSource();
  m_wantDraw = true;
}

void Renderer::UpdateData() {
  CELLNTA_LOG_DEBUG("Updating render data");
  m_cells.clear();
  for (const auto& data : m_renderData) {
    const RenderData::ChunkPtr& chunk = data.second;

    if (chunk == nullptr)
      continue;

    for (auto& pos : *chunk) {
      if (pos.second != 0)
        AddCell<false>(pos.first);
    }
  }

  m_updateVboCells = true;
  ProjectBuffers();
}

void Renderer::UpdateColorTexture() {
  switch (m_cube.GetMode()) {
    case CubeMode::POINTS:
      m_color.Generate(m_cube.GetVerticesCount(), 1);
      break;
    case CubeMode::WIREFRAME:
      if (m_cube.GetDimensions() == 1)
        m_color.Generate(1, 1);
      else
        m_color.Generate(m_cube.GetFacesCount() * 2, 1);
      break;
    case CubeMode::POLYGON: m_color.Generate(m_cube.GetFacesCount(), 2); break;
    default: assert(0 && "Unreachable"); break;
  }

  glBindBuffer(GL_TEXTURE_BUFFER, m_colorBuffer);
  glBufferData(GL_TEXTURE_BUFFER, m_color.GetSizeInBytes(), m_color.GetData(),
               GL_DYNAMIC_DRAW);
  m_wantDraw = true;
  m_color.Clear();
}
