#pragma once

#include <Eigen/Core>

#include <ActionBase.h>

namespace Ui {
namespace Action {

namespace Camera3d {

class SetMouseSpeed : public Action::Base {
 public:
  SetMouseSpeed(float speed) : m_speed(speed) {}
  void Execute() override;

 private:
  float m_speed;
};

class SetMoveSpeed : public Action::Base {
 public:
  SetMoveSpeed(float speed) : m_speed(speed) {}
  void Execute() override;

 private:
  float m_speed;
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

class SetUsePerspective : public Action::Base {
 public:
  SetUsePerspective(bool perspective) : m_perspective(perspective) {}
  void Execute() override;

 private:
  int m_perspective;
};

}  // namespace Camera3d

namespace CameraNd {

class ToggleNeedSkip : public Action::Base {
 public:
  ToggleNeedSkip(int idx) : m_idx(idx) {}
  void Execute() override;

 private:
  int m_idx;
};

class SetNeedSkip : public Action::Base {
 public:
  SetNeedSkip(int idx, bool skip) : m_idx(idx), m_skip(skip) {}
  void Execute() override;

 private:
  int m_idx;
  bool m_skip;
};

class SetPosition : public Action::Base {
 public:
  SetPosition(int idx, const Eigen::VectorXf& pos) : m_idx(idx), m_pos(pos) {}
  void Execute() override;

 private:
  int m_idx;
  Eigen::VectorXf m_pos;
};

class SetFront : public Action::Base {
 public:
  SetFront(int idx, const Eigen::VectorXf& front)
      : m_idx(idx), m_front(front) {}
  void Execute() override;

 private:
  int m_idx;
  Eigen::VectorXf m_front;
};

class SetUsePerspective : public Action::Base {
 public:
  SetUsePerspective(int idx, bool perspective)
      : m_idx(idx), m_perspective(perspective) {}
  void Execute() override;

 private:
  int m_idx;
  int m_perspective;
};

}  // namespace CameraNd

}  // namespace Action
}  // namespace Ui
