#include "Cellnta/Renderer/Renderer.h"

#include <type_traits>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/Renderer/Transform.h"
#include "Cellnta/Renderer/Camera3d.h"
#include "Cellnta/Renderer/CameraNd.h"

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

  if (m_data.NeedUpdate()) {
    m_updateVboCells = true;
    ProjectBuffers();
    m_data.Handled();
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

  m_wantDraw = false;

  NCellStorage& cells = m_data.GetCells();

  if (cells.size() == 0)
    return;

  m_cellShader.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_BUFFER, m_colorTexture);

  GLenum renderMode = 0;
  switch (m_cube.GetMode()) {
    case CubeMode::POINTS: renderMode = GL_POINTS; break;
    case CubeMode::WIREFRAME: renderMode = GL_LINES; break;
    case CubeMode::POLYGON: renderMode = GL_TRIANGLES; break;
    default: assert(0 && "Unreachable"); break;
  }

  static_assert(std::is_same<HypercubeStorage::Ind, uint16_t>::value,
                "Indices in glDrawElement has another type");
  glDrawElementsInstanced(renderMode, m_cube.GetIndicesSize(),
                          GL_UNSIGNED_SHORT, nullptr, cells.size());
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

void Renderer::Rotate() {
  if (m_cameraNd == nullptr)
    return;

  m_cameraNd->at(m_cameraNd->size() - 1).Rotate();
  // Eigen::MatrixXf mat;
  // Eigen::Rotation2D rot(angle);

  // const int dim = m_cameraNd.end()->GetDimensions();
  // NCamera& cam = *m_cameraNd.end();
  // cam. = NRotate(dim, 0, dim - 1, radians(5.0f));
}

void Renderer::SetDimension(uint32_t dim) {
  CELLNTA_PROFILE;

  m_d = dim;
  GenrateHypercube(m_cube.GetScale());

  m_data.SetDimension(dim);

  if (m_cameraNd == nullptr) {
    CELLNTA_LOG_TRACE("Unable to set dimension in Nd camera");
    return;
  }

  m_cameraNd->clear();

  for (uint32_t i = dim; i > 3; --i) {
    Eigen::VectorXf pos = Eigen::VectorXf::Zero(i);
    Eigen::VectorXf front = Eigen::VectorXf::Zero(i);
    CameraNd cam;

    pos(pos.rows() - 1) = 2.0f;
    cam.SetDimensions(i);
    cam.SetPosition(pos);
    cam.SetFront(front);

    m_cameraNd->push_back(cam);
  }
}

void Renderer::SetCubeMode(CubeMode mode) {
  GenrateHypercube(-1, mode);
}

void Renderer::SetRenderDistance(uint32_t distance) {
  CELLNTA_PROFILE;

  m_data.SetDistance(distance);
  UpdateRenderDistanceUniform();
}

void Renderer::ProjectBuffers() {
  CELLNTA_PROFILE;

  if (m_cameraNd == nullptr)
    return;

  NCellStorage& cells = m_data.GetCells();

  m_cube.Restore();
  cells.Restore();

  for (auto& camera : *m_cameraNd) {
    if (camera.WantSkip())
      continue;

    const Eigen::MatrixXf viewProj = camera.CalculateViewProj();
    const int dim = camera.GetDimensions();
    const bool usePerspective = camera.GetUsePerspective();

    NProject(m_cube, dim, viewProj, usePerspective);
    NProject(cells, dim, viewProj, usePerspective);
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
  static_assert(std::is_same<ColorStorage::Type, float>::value,
                "glTexBuffer has another type");
  static_assert(ColorStorage::SIZE == 4, "glTexBuffer has another size");
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_colorBuffer);

  glBindVertexArray(m_vao);

  static_assert(std::is_same<HypercubeStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeBuffer);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(HypercubeStorage::Point), nullptr);
  glEnableVertexAttribArray(0);

  static_assert(std::is_same<NCellStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(NCellStorage::Point), nullptr);
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

  if (m_camera3d == nullptr)
    return;

  const Eigen::Matrix4f& proj = m_camera3d->GetProjection();
  const Eigen::Matrix4f& view = m_camera3d->GetView();
  const Eigen::Matrix4f projView = proj * view;

  m_cellShader.Use();
  m_cellShader.Set("u_projView", projView);

  m_gridShader.Use();
  m_gridShader.Set("u_projView", projView);

  m_wantDraw = true;
}

void Renderer::UpdateRenderDistanceUniform() {
  CELLNTA_PROFILE;

  constexpr float FarPlane = 6.25f;

  m_gridShader.Use();
  m_gridShader.Set("u_near", 0.01f);
  m_gridShader.Set("u_far", (float)m_data.GetDistance() * FarPlane);
}

void Renderer::UpdateCamera() {
  CELLNTA_PROFILE;

  UpdateCamera3d();
  UpdateCameraNd();
}

void Renderer::UpdateCamera3d() {
  CELLNTA_PROFILE;

  if (m_camera3d == nullptr) {
    CELLNTA_LOG_TRACE("Unable to update 3d camera");
    return;
  }

  if (m_camera3d->Update()) {
    m_data.SetPosition(m_camera3d->GetPosition().cast<int>());
    UpdateCameraUniform();
  }
}

void Renderer::UpdateCameraNd() {
  CELLNTA_PROFILE;

  if (m_cameraNd == nullptr) {
    CELLNTA_LOG_TRACE("Unable to update Nd camera");
    return;
  }

  bool camUpdated = false;
  for (auto& ncam : *m_cameraNd)
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
    dst[0] = pnt[m_data.GetCollatingX()];
    dst[1] = pnt[m_data.GetCollatingY()];
    dst[2] = pnt[m_data.GetCollatingZ()];

    pnt += m_cube.GetVertexSize();
    dst += 3;
  }
  EndArrayBufferSource();
  m_wantDraw = true;
}

void Renderer::UpdateCellBuffer() {
  CELLNTA_PROFILE;

  NCellStorage::VecList& cells = m_data.GetCells().GetRaw();

  CELLNTA_LOG_TRACE("Updating cell buffer");
  if (cells.empty())
    return;

  float* dst = nullptr;
  int pointsCount = cells.size() * 3;

  glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);

  if (cells.capacity() != m_oldCellsCapacity) {
    m_oldCellsCapacity = cells.capacity();
    glBindBuffer(GL_ARRAY_BUFFER, m_cellBuffer);
    glBufferData(GL_ARRAY_BUFFER, 3 * cells.capacity() * sizeof(NCellStorage::Point), nullptr,
                 GL_DYNAMIC_DRAW);
  }

  BeginArrayBufferSource(dst, 0, pointsCount * sizeof(NCellStorage::Point));

  const float* end = dst + pointsCount;
  Eigen::VectorXf* pnt = cells.data();
  if (pnt == nullptr) {
    EndArrayBufferSource();
    return;
  }

  // memset(dst, 0, GetTotalAllocatedMemoryForPoints());
  while (dst < end) {
    dst[0] = (*pnt)(m_data.GetCollatingX());
    dst[1] = (*pnt)(m_data.GetCollatingY());
    dst[2] = (*pnt)(m_data.GetCollatingZ());
    ++pnt;
    dst += 3;
  }

  EndArrayBufferSource();
  m_wantDraw = true;
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
