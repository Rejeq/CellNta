#include "Cellnta/Renderer/Renderer.h"

#include <type_traits>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/Renderer/Camera3d.h"
#include "Cellnta/Renderer/CameraNd.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/RenderData.h"
#include "Cellnta/Renderer/Transform.h"

using namespace Cellnta;

Renderer::Renderer() {
  CELLNTA_PROFILE;

  glGenVertexArrays(1, &m_vao);
}

Renderer::~Renderer() {
  CELLNTA_PROFILE;

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

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

  if (m_cube != nullptr) {
    if (m_cube->NeedUpdatePoints()) {
      ProjectBuffers(true, false);
      m_wantDraw = true;
      m_cube->PointsHandled();
    }

    if (m_cube->NeedUpdateIndices()) {
      m_cubeDrawer.UpdateIndices(*m_cube);
      m_wantDraw = true;
      m_cube->IndicesHandled();
    }

    ColorStorage& color = m_cube->GetColor();
    if (color.NeedUpdate()) {
      m_cubeDrawer.GetColor().Update(color);
      m_wantDraw = true;
      color.Handled();
    }
  }

  if (m_data != nullptr) {
    NCellStorage& cells = m_data->GetCells();
    if (cells.NeedUpdate()) {
      ProjectBuffers(false, true);
      m_wantDraw = true;
      cells.Handled();
    }
  }
}

void Renderer::DrawGrid() {
  CELLNTA_PROFILE;

  m_gridShader.Use();
  glDrawArrays(GL_TRIANGLES, 0, 12);
}

void Renderer::Draw() {
  CELLNTA_PROFILE;

  if (m_cube == nullptr || m_data == nullptr) {
    CELLNTA_LOG_TRACE("Unable to draw HypercubeStorage or RenderData");
    return;
  }

  m_wantDraw = false;
  NCellStorage& cells = m_data->GetCells();
  if (cells.size() == 0)
    return;

  m_cellShader.Use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_BUFFER, m_cubeDrawer.GetColor().GetTexture());

  GLenum renderMode = 0;
  switch (m_cube->GetMode()) {
    case CubeMode::POINTS: renderMode = GL_POINTS; break;
    case CubeMode::WIREFRAME: renderMode = GL_LINES; break;
    case CubeMode::POLYGON: renderMode = GL_TRIANGLES; break;
    default: assert(0 && "Unreachable"); break;
  }

  static_assert(std::is_same<HypercubeStorage::Ind, uint16_t>::value,
                "Indices in glDrawElement has another type");
  glDrawElementsInstanced(renderMode, m_cube->GetIndicesSize(),
                          GL_UNSIGNED_SHORT, nullptr, cells.size());
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

  if (m_cube != nullptr)
    m_cube->SetDimension(dim);
  if (m_data != nullptr)
    m_data->SetDimension(dim);

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

void Renderer::SetRenderDistance(uint32_t distance) {
  CELLNTA_PROFILE;

  if (m_data == nullptr)
    return;

  m_data->SetDistance(distance);
  UpdateRenderDistanceUniform();
}

void Renderer::ProjectBuffers(bool projectCube, bool projectCells) {
  CELLNTA_PROFILE;

  if (m_cameraNd == nullptr || m_cube == nullptr || m_data == nullptr)
    return;

  CELLNTA_LOG_TRACE("Project buffers: cube = {}, cells = {}", projectCube,
                    projectCells);

  if (!projectCube && !projectCells) {
    CELLNTA_LOG_TRACE("Both buffers are equal to false");
    return;
  }

  NCellStorage& cells = m_data->GetCells();

  m_cube->Restore();
  cells.Restore();

  for (auto& camera : *m_cameraNd) {
    if (camera.WantSkip())
      continue;

    const Eigen::MatrixXf viewProj = camera.CalculateViewProj();
    const int dim = camera.GetDimensions();
    const bool usePerspective = camera.GetUsePerspective();

    if (projectCube)
      NProject(*m_cube, dim, viewProj, usePerspective);
    if (projectCells)
      NProject(cells, dim, viewProj, usePerspective);
  }

  if (projectCube)
    m_cubeDrawer.UpdatePoints(*m_cube);
  if (projectCells)
    m_cellDrawer.Update(cells);
  m_wantDraw = true;
}

void Renderer::SetHypercube(const std::shared_ptr<HypercubeStorage>& cube) {
  CELLNTA_PROFILE;

  m_cube = cube;

  if (m_cube == nullptr)
    return;

  glBindVertexArray(m_vao);

  static_assert(std::is_same<HypercubeStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cubeDrawer.GetPointsBuffer());
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(HypercubeStorage::Point), nullptr);
  glEnableVertexAttribArray(0);
}

void Renderer::SetData(const std::shared_ptr<RenderData>& data) {
  CELLNTA_PROFILE;

  m_data = data;

  if (m_data == nullptr)
    return;

  glBindVertexArray(m_vao);

  static_assert(std::is_same<NCellStorage::Point, float>::value,
                "glVertexAttribute has another type");
  glBindBuffer(GL_ARRAY_BUFFER, m_cellDrawer.GetBuffer());
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(NCellStorage::Point), nullptr);
  glVertexAttribDivisor(1, 1);
  glEnableVertexAttribArray(1);
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

  if (m_data == nullptr)
    return;

  m_gridShader.Use();
  m_gridShader.Set("u_near", 0.01f);
  m_gridShader.Set("u_far", (float)m_data->GetDistance() * FarPlane);
}

void Renderer::UpdateCamera() {
  CELLNTA_PROFILE;

  UpdateCamera3d();
  UpdateCameraNd();
}

void Renderer::UpdateCamera3d() {
  CELLNTA_PROFILE;

  if (m_camera3d == nullptr || m_data == nullptr) {
    CELLNTA_LOG_TRACE("Unable to update 3d camera");
    return;
  }

  if (m_camera3d->Update()) {
    m_data->SetPosition(m_camera3d->GetPosition().cast<int>());
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
  if (camUpdated) {
    ProjectBuffers();
  }
}
