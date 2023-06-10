#pragma once

#include <Eigen/Core>

#include <ActionBase.h>

namespace Ui::Action::CameraNd {

class ToggleNeedSkip : public Action::UndoBase {
 public:
  ToggleNeedSkip(int idx) : m_idx(idx) {}
  void Execute() override;
  void Undo() override { Execute(); }

 private:
  int m_idx;
};

class SetNeedSkip : public Action::UndoBase {
 public:
  SetNeedSkip(int idx, bool skip) : m_idx(idx), m_skip(skip) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_idx;
  bool m_skip;
  bool m_prevSkip;
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

class SetUsePerspective : public Action::UndoBase {
 public:
  SetUsePerspective(int idx, bool perspective)
      : m_idx(idx), m_perspective(perspective) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_idx;
  int m_perspective;
  int m_prevPerspective;
};

}  // namespace Ui::Action::CameraNd
