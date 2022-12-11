#include "Cellnta/Renderer/CameraNd.h"

#include "Cellnta/Renderer/Transform.h"

using namespace Cellnta;

void CameraNd::SetDimensions(int dim) {
  CELLNTA_PROFILE;

  m_front.resize(dim);
  m_pos.resize(dim);
  m_proj.resize(GetVecSize(), GetVecSize());
  m_view.resize(GetVecSize(), GetVecSize());

  m_ups = Eigen::MatrixXf::Zero(dim, dim - 2);
  // This creates the Up vectors in an order that will not rotate the axes
  m_ups.block(1, 0, dim - 3, dim - 3) =
      Eigen::MatrixXf::Identity(dim - 3, dim - 3).rowwise().reverse();
  m_ups(dim - 1, dim - 3) = 1.f;

  for (int i = 0; i < m_ups.cols(); i++)
    m_ups(0, i) += 1.e-20f;

  m_dim = dim;
}

void CameraNd::UpdateViewMatrix() {
  m_view = LookAt(m_dim, m_pos, m_front, m_ups);
}

void CameraNd::UpdateProjMatrix() {
  m_proj = Perspective(m_dim, m_fov, 0.0f, 0.0f, 0.0f);
}

Eigen::MatrixXf CameraNd::CalculateViewProj() const {
  CELLNTA_PROFILE;

  Eigen::MatrixXf result = m_proj * m_view;

  // Axis direction correction
  Eigen::MatrixXf aux = Eigen::MatrixXf::Identity(GetVecSize(), GetVecSize());
  // X corrections
  aux(0, 0) = -1;
  // Z correction
  aux(2, 2) = m_dim != 4 ? -1 : 1;
  result = aux * result;

  return result;
}

void CameraNd::NeedSkip(bool needSkip) {
  m_wantSkip = needSkip;
  p_updateProj = true;
  p_updateView = true;
}

void CameraNd::SetView(const Eigen::MatrixXf& view) {
  m_view = view;
  p_updateMatrix = true;
}

void CameraNd::SetProjection(const Eigen::MatrixXf& proj) {
  m_proj = proj;
  p_updateMatrix = true;
}

void CameraNd::SetPosition(const Eigen::VectorXf& pos) {
  m_pos = pos;
  p_updateView = true;
}

void CameraNd::SetFront(const Eigen::VectorXf& front) {
  m_front = front;
  p_updateView = true;
}

void CameraNd::SetUp(const Eigen::MatrixXf& Ups) {
  m_ups = Ups;
  p_updateView = true;
}

void CameraNd::SetFov(float fov) {
  m_fov = fov;
  p_updateView = true;
}

void CameraNd::Rotate() {
  m_view *= NRotate(m_dim, m_dim - 2, m_dim - 1,
                    radians(5.0f));
  p_updateMatrix = true;
}
