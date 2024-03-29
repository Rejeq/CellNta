#pragma once

#include <Eigen/Core>

#include <ActionBase.h>

namespace Ui::Action::Camera3d {

class SetMouseSpeed : public Action::UndoBase {
 public:
  SetMouseSpeed(float speed) : m_speed(speed) {}
  void Execute() override;
  void Undo() override;

 private:
  float m_speed;
  float m_prevSpeed;
};

class SetMoveSpeed : public Action::UndoBase {
 public:
  SetMoveSpeed(float speed) : m_speed(speed) {}
  void Execute() override;
  void Undo() override;

 private:
  float m_speed;
  float m_prevSpeed;
};

class SetPosition : public Action::Base {
 public:
  SetPosition(const Eigen::Vector3f& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::Vector3f m_pos;
};

class SetYaw : public Action::Base {
 public:
  SetYaw(float yaw) : m_yaw(yaw) {}
  void Execute() override;

 private:
  float m_yaw;
};

class SetPitch : public Action::Base {
 public:
  SetPitch(float pitch) : m_pitch(pitch) {}
  void Execute() override;

 private:
  float m_pitch;
};

class SetUsePerspective : public Action::UndoBase {
 public:
  SetUsePerspective(bool perspective) : m_perspective(perspective) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_perspective;
  int m_prevPerspective;
};

}  // namespace Ui::Action::Camera3d
