#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/CameraBase.h"

namespace Cellnta {

enum class MoveDirection
{
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  WORLD_UP,
  WORLD_DOWN,
};

class Camera3d : public CameraBase
{
public:
  Camera3d() : CameraBase(3) {}

  void Move(MoveDirection dir, float delta);
  void Rotate(float xoffset, float yoffset, float delta);
  void Resize(Eigen::Vector2f size) { m_size = size; p_updateProj = true; }

  void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
  void SetMouseSpeed(float speed) { m_mouseSpeed = speed; }

  void SetYawBase(float yaw) { m_yaw = yaw; p_updateView = true; }
  void SetPitchBase(float pitch) { m_pitch = pitch; p_updateView = true; }

  const float& GetMoveSpeed() const { return m_moveSpeed; }
  const float& GetMouseSpeed() const { return m_mouseSpeed; }
  float& GetMoveSpeed() { return m_moveSpeed; }
  float& GetMouseSpeed() { return m_mouseSpeed; }

  const Eigen::Matrix4f& GetViewMatrixBase() const { return m_view; }
  const Eigen::Matrix4f& GetProjectionMatrixBase() const { return m_proj; }
  const Eigen::Vector3f& GetPositionBase() const { return m_pos; }
  const Eigen::Vector3f& GetFrontBase() const { return m_front; }
  const Eigen::Vector2f& GetSizeBase() const { return m_size; }
  float GetYawBase() const { return m_yaw; }
  float GetPitchBase() const { return m_pitch; }

  void SetUsePerspective(bool newState) override { CameraBase::SetUsePerspective(newState); p_updateProj = true; }

  void SetViewMatrix(const Eigen::MatrixXf& view) override { m_view = view; p_updateMatrix = true; }
  void SetProjectionMatrix(const Eigen::MatrixXf& proj) override { m_proj = proj; p_updateMatrix = true; }
  void SetPosition(const Eigen::VectorXf& pos) override { m_pos = pos; p_updateView = true; }
  void SetFront(const Eigen::VectorXf& front) override { m_front = front; p_updateView = true; }
  void SetWorldUp(const Eigen::MatrixXf& worldUp) override { m_worldUp = worldUp; p_updateView = true; }
  void SetFov(float fov) override { m_fov = fov; p_updateView = true; }

  Eigen::MatrixXf GetViewMatrix() const override { return m_view; }
  Eigen::MatrixXf GetProjectionMatrix() const override { return m_proj; }
  Eigen::MatrixXf GetUps() const override { return m_worldUp; }
  Eigen::VectorXf GetPosition() const override { return m_pos; }
  Eigen::VectorXf GetFront() const override { return m_front; }

  //TODO:
  void ForceUpdateProj() { p_updateProj = true; }

private:
  void UpdateViewMatrix() override;
  void UpdateProjMatrix() override;

  float m_moveSpeed = 15.0f;
  float m_mouseSpeed = 4.0f;

  Eigen::Vector3f m_pos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
  Eigen::Vector3f m_front = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
  Eigen::Vector3f m_worldUp = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
  Eigen::Vector2f m_size = Eigen::Vector2f(400.0f, 400.0f);//TODO: move it
  float m_fov = radians(45.0f);

  Eigen::Vector3f m_right;
  Eigen::Vector3f m_up;

  float m_yaw = 0.0f;
  float m_pitch = 0.0f;

  Eigen::Matrix4f m_view;
  Eigen::Matrix4f m_proj;
};

} //namespace Cellnta
