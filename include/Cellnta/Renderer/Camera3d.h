#pragma once

#include <Eigen/Dense>

#include "Cellnta/Renderer/CameraController.h"

namespace Cellnta {

enum class MoveDirection {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  WORLD_UP,
  WORLD_DOWN,
};

class Camera3d : public CameraController {
 public:
  Camera3d() = default;

  void Move(MoveDirection dir, float delta);
  void Rotate(float xoffset, float yoffset, float delta);
  void Resize(Eigen::Vector2f size);

  void SetMoveSpeed(float speed) { m_moveSpeed = speed; }
  void SetMouseSpeed(float speed) { m_mouseSpeed = speed; }
  float GetMoveSpeed() const { return m_moveSpeed; }
  float GetMouseSpeed() const { return m_mouseSpeed; }

  const Eigen::Matrix4f& GetView() const { return m_view; }
  const Eigen::Matrix4f& GetProjection() const { return m_proj; }
  const Eigen::Vector3f& GetPosition() const { return m_pos; }
  const Eigen::Vector3f& GetFront() const { return m_front; }
  const Eigen::Vector2f& GetSize() const { return m_size; }
  float GetFov() const { return m_fov; }
  float GetYaw() const { return m_yaw; }
  float GetPitch() const { return m_pitch; }

  void SetView(const Eigen::Matrix4f& view);
  void SetProjection(const Eigen::Matrix4f& proj);
  void SetPosition(const Eigen::Vector3f& pos);
  void SetFront(const Eigen::Vector3f& front);
  void SetWorldUp(const Eigen::Vector3f& worldUp);
  void SetFov(float fov);
  void SetYaw(float yaw);
  void SetPitch(float pitch);

 private:
  void UpdateViewMatrix() override;
  void UpdateProjMatrix() override;

  float m_moveSpeed = 15.0f;
  float m_mouseSpeed = 4.0f;

  Eigen::Vector3f m_pos = Eigen::Vector3f(0.0f, 0.0f, 0.0f);
  Eigen::Vector3f m_front = Eigen::Vector3f(0.0f, 0.0f, -1.0f);
  Eigen::Vector3f m_worldUp = Eigen::Vector3f(0.0f, 1.0f, 0.0f);
  Eigen::Vector2f m_size = Eigen::Vector2f(400.0f, 400.0f);  // TODO: move it
  float m_fov = radians(45.0f);

  Eigen::Vector3f m_right;
  Eigen::Vector3f m_up;

  float m_yaw = 0.0f;
  float m_pitch = 0.0f;

  Eigen::Matrix4f m_view;
  Eigen::Matrix4f m_proj;
};

}  // namespace Cellnta
