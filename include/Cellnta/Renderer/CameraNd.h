#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/CameraController.h"
// TODO: Delete this
#include "Cellnta/Renderer/Transform.h"

namespace Cellnta {

class CameraNd : public CameraController {
 public:
  CameraNd() = default;

  void NeedSkip(bool needSkip);
  bool WantSkip() const { return m_wantSkip; }

  void SetDimensions(int dim);
  int GetDimensions() const { return m_dim; }

  Eigen::MatrixXf CalculateViewProj() const;

  void SetView(const Eigen::MatrixXf& view);
  void SetProjection(const Eigen::MatrixXf& proj);
  void SetPosition(const Eigen::VectorXf& pos);
  void SetFront(const Eigen::VectorXf& front);
  void SetUp(const Eigen::MatrixXf& Ups);
  void SetFov(float fov);

  const Eigen::MatrixXf& GetView() const { return m_view; }
  const Eigen::MatrixXf& GetProjection() const { return m_proj; }
  const Eigen::MatrixXf& GetUp() const { return m_ups; }
  const Eigen::VectorXf& GetPosition() const { return m_pos; }
  const Eigen::VectorXf& GetFront() const { return m_front; }

  // TODO: make usable
  void Rotate();

 private:
  void UpdateViewMatrix() override;
  void UpdateProjMatrix() override;

  int GetVecSize() const { return m_dim + 1; }

  Eigen::VectorXf m_pos;
  Eigen::VectorXf m_front;
  Eigen::MatrixXf m_ups;

  Eigen::MatrixXf m_view;
  Eigen::MatrixXf m_proj;

  bool m_wantSkip = false;
  float m_fov = 45.0f;  // radians(45.0f);
  int m_dim = 0;
};

}  // namespace Cellnta
