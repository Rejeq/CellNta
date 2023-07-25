#include "Cellnta/Renderer/Renderer.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/Renderer/Camera3d.h"
#include "Cellnta/Renderer/CameraNd.h"
#include "Cellnta/Renderer/HypercubeStorage.h"
#include "Cellnta/Renderer/RenderData.h"
#include "Cellnta/Renderer/Transform.h"

using namespace Cellnta;

bool Renderer::Init(DrawerBackend backend) {
  m_sceneDrawer = CreateSceneDrawerInstance(backend);
  if (m_sceneDrawer == nullptr) {
    CELLNTA_LOG_ERROR("Unable to create scene drawer instance in Renderer");
    return true;
  }
  return false;
}

bool Renderer::CreateShaders(const std::string& gridPath,
                             const std::string& cellPath) {
  CELLNTA_PROFILE;

  return m_sceneDrawer->CreateShaders(gridPath, cellPath);
}

void Renderer::Update() {
  CELLNTA_PROFILE;

  UpdateCamera();

  if (m_cube != nullptr) {
    if (m_cube->NeedUpdatePoints() || NeedForceUpdateCubePoints()) {
      ProjectBuffers(true, false);
      m_wantDraw = true;
      m_cube->PointsHandled();
    }

    if (m_cube->NeedUpdateIndices()) {
      m_sceneDrawer->GetCube().UpdateIndices(*m_cube);
      m_wantDraw = true;
      m_cube->IndicesHandled();
    }

    ColorStorage& color = m_cube->GetColor();
    if (color.NeedUpdate()) {
      m_sceneDrawer->GetCube().GetColor().Update(color);
      m_wantDraw = true;
      color.Handled();
    }
  }

  if (m_data != nullptr) {
    NCellStorage& cells = m_data->GetCells();
    if (cells.NeedUpdate() || NeedForceUpdateCells()) {
      ProjectBuffers(false, true);
      m_wantDraw = true;
      cells.Handled();
    }
  }

  m_prevIgnCubeProj = m_ignCubeProj;
  m_prevIgnCellProj = m_ignCellProj;
}

void Renderer::DrawGrid() {
  CELLNTA_PROFILE;

  m_sceneDrawer->DrawGrid();
}

void Renderer::Draw() {
  CELLNTA_PROFILE;

  m_wantDraw = false;
  m_sceneDrawer->Draw();
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
  m_sceneDrawer->UpdateDistance(distance);
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

  if (projectCube)
    m_cube->Restore();

  if (projectCells)
    cells.RestoreVisible();

  for (auto& camera : *m_cameraNd) {
    if (camera.WantSkip())
      continue;

    const Eigen::MatrixXf viewProj = camera.CalculateViewProj();
    const int dim = camera.GetDimensions();
    const bool usePerspective = camera.GetUsePerspective();

    if (projectCube && !m_ignCubeProj)
      NProject(*m_cube, dim, viewProj, usePerspective);

    if (projectCells && !m_ignCellProj)
      NProject(cells, dim, viewProj, usePerspective);
  }

  if (projectCube)
    m_sceneDrawer->GetCube().UpdatePoints(*m_cube);
  if (projectCells)
    m_sceneDrawer->GetCell().Update(cells);
  m_wantDraw = true;
}

void Renderer::SetHypercube(const std::shared_ptr<HypercubeStorage>& cube) {
  CELLNTA_PROFILE;

  m_cube = cube;
}

void Renderer::SetData(const std::shared_ptr<RenderData>& data) {
  CELLNTA_PROFILE;

  m_data = data;
}

void Renderer::SetIgnoreCubeProject(bool newState) {
  m_prevIgnCubeProj = m_ignCubeProj;
  m_ignCubeProj = newState;
}

void Renderer::SetIgnoreCellProject(bool newState) {
  m_prevIgnCellProj = m_ignCellProj;
  m_ignCellProj = newState;
}

void Renderer::UpdateCameraUniform() {
  CELLNTA_PROFILE;

  if (m_camera3d == nullptr)
    return;

  const Eigen::Matrix4f& proj = m_camera3d->GetProjection();
  const Eigen::Matrix4f& view = m_camera3d->GetView();
  const Eigen::Matrix4f projView = proj * view;
  m_sceneDrawer->UpdateCamera(projView);
  m_wantDraw = true;
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

bool Renderer::NeedForceUpdateCubePoints() const {
  return m_ignCubeProj != m_prevIgnCubeProj;
}

bool Renderer::NeedForceUpdateCells() const {
  return m_ignCellProj != m_prevIgnCellProj;
}
