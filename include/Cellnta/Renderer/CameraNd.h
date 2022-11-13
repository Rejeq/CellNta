#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/CameraBase.h"

//TODO: Delete this
#include "Cellnta/Renderer/Transform.h"

namespace Cellnta {

class CameraNd : public CameraBase
{
public:
  void SetNeedSkip(bool skip) { m_needSkip = skip; p_updateProj = true; p_updateView = true; }
  bool NeedSkip() const { return m_needSkip; }

  void SetDimensions(size_t dim) override;
  void SetUsePerspective(bool newState) override;

  void SetViewMatrix(const Eigen::MatrixXf& view) override { m_view = view; p_updateMatrix = true; }
  void SetProjectionMatrix(const Eigen::MatrixXf& proj) override { m_proj = proj; p_updateMatrix = true; }
  void SetPosition(const Eigen::VectorXf& pos) override { m_pos = pos; p_updateView = true; }
  void SetFront(const Eigen::VectorXf& front) override { m_front = front; p_updateView = true; }
  //TODO: Rename
  void SetWorldUp(const Eigen::MatrixXf& Ups) override { m_ups = Ups; p_updateView = true; }
  void SetFov(float fov) override { p_fov = fov; p_updateView = true; }

  Eigen::MatrixXf GetViewProj() const { return CalculateViewProj(); }

  Eigen::MatrixXf GetViewMatrix() const override { return m_view; }
  Eigen::MatrixXf GetProjectionMatrix() const override { return m_proj; }
  Eigen::MatrixXf GetUps() const override { return m_ups; }
  Eigen::VectorXf GetPosition() const override { return m_pos; }
  Eigen::VectorXf GetFront() const override { return m_front; }

  void Rotate() { m_view *= NRotate(GetDimensions(), GetDimensions() - 2, GetDimensions() - 1, radians(5.0f)); p_updateMatrix = true; }

private:
  void UpdateViewMatrix() override;
  void UpdateProjMatrix() override;

  Eigen::MatrixXf CalculateViewProj() const;

  bool m_needSkip = false;

  Eigen::VectorXf m_pos;
  Eigen::VectorXf m_front;
  Eigen::MatrixXf m_ups;

  Eigen::MatrixXf m_view;
  Eigen::MatrixXf m_proj;
};

} //namespace Cellnta
