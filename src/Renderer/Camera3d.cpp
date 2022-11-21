#include "Cellnta/Renderer/Camera3d.h"

#include "Cellnta/Config.h"

using namespace Cellnta;

void Camera3d::Move(MoveDirection dir, float delta)
{
  CELLNTA_PROFILE;

  p_updateView = true;
  const float velocity = m_moveSpeed * delta;
  switch (dir)
  {
  case MoveDirection::FORWARD:    m_pos += m_front * velocity; break;
  case MoveDirection::BACKWARD:   m_pos -= m_front * velocity; break;
  case MoveDirection::LEFT:       m_pos -= m_right * velocity; break;
  case MoveDirection::RIGHT:      m_pos += m_right * velocity; break;
  case MoveDirection::WORLD_UP:   m_pos += m_worldUp * velocity; break;
  case MoveDirection::WORLD_DOWN: m_pos -= m_worldUp * velocity; break;
  }
}

void Camera3d::Rotate(float xoffset, float yoffset, float delta)
{
  CELLNTA_PROFILE;

  p_updateView = true;

  xoffset *= m_mouseSpeed * delta;
  yoffset *= m_mouseSpeed * delta;

  m_yaw += radians(xoffset);
  m_pitch -= radians(yoffset);

  constexpr float quarter = ((float)EIGEN_PI) / 2.0f;
  if (m_pitch > quarter)
    m_pitch = quarter - 0.001f;
  else if (m_pitch < -quarter)
    m_pitch = -quarter + 0.001f;
}

void Camera3d::UpdateViewMatrix()
{
  CELLNTA_PROFILE;

  const double cYaw = cos(m_yaw);
  const double cPitch = cos(m_pitch);
  const double sYaw = sin(m_yaw);
  const double sPitch = sin(m_pitch);

  m_front.x() = cYaw * cPitch;
  m_front.y() = sPitch;
  m_front.z() = sYaw * cPitch;

  m_front.normalize();
  m_right = m_front.cross(m_worldUp).normalized();
  m_up = m_right.cross(m_front).normalized();

  //m_view = glm::lookAt(m_pos, m_pos + m_front, m_up);

  Eigen::Vector3f f(m_front.normalized());
  const Eigen::Vector3f s(f.cross(m_up).normalized());
  const Eigen::Vector3f u(s.cross(f));

  m_view = Eigen::Matrix4f::Identity();
  m_view(0, 0) = s.x();
  m_view(0, 1) = s.y();
  m_view(0, 2) = s.z();
  m_view(1, 0) = u.x();
  m_view(1, 1) = u.y();
  m_view(1, 2) = u.z();
  m_view(2, 0) = -f.x();
  m_view(2, 1) = -f.y();
  m_view(2, 2) = -f.z();
  m_view(0, 3) = -s.dot(m_pos);
  m_view(1, 3) = -u.dot(m_pos);
  m_view(2, 3) = f.dot(m_pos);
}


void Camera3d::UpdateProjMatrix()
{
  CELLNTA_PROFILE;

  if (p_usePerspective)
  {
    //Perspective projection
    const float zNear = 0.001f;
    const float zFar = 1000.0f;
    const float aspect = m_size.x() / m_size.y();
    const float tanHalfFovy = tan(m_fov / 2.0f);

    m_proj = Eigen::Matrix4f::Zero();
    m_proj(0, 0) = 1.0f / (aspect * tanHalfFovy);
    m_proj(1, 1) = 1.0f / (tanHalfFovy);
    m_proj(2, 2) = -(zFar + zNear) / (zFar - zNear);
    m_proj(3, 2) = -1.0f;
    m_proj(2, 3) = -(2.0f * zFar * zNear) / (zFar - zNear);
  }
  else
  {
    //orthographic projection
    const float left = -2.0f;
    const float right = +2.0f;
    const float bottom = -2.0f;
    const float top = +2.0f;
    const float zNear = -0.001f;
    const float zFar = +100.0f;

    m_proj = m_proj.Identity();
    m_proj(0, 0) = 2.0f / (right - left);
    m_proj(1, 1) = 2.0f / (top - bottom);
    m_proj(2, 2) = -2.0f / (zFar - zNear);
    m_proj(0, 3) = -(right + left) / (right - left);
    m_proj(1, 3) = -(top + bottom) / (top - bottom);
    m_proj(2, 3) = -(zFar + zNear) / (zFar - zNear);
  }
}
