#pragma once

#include <Cellnta/Area.h>
#include <Cellnta/Snapshot.h>
#include <Cellnta/World/WorldList.h>

#include "ActionBase.h"

namespace Ui {
namespace Action {
namespace Pattern {

class Base : public Action::UndoBase {
 public:
  void Undo() override;

 protected:
  void MakeUndoAble(const Cellnta::Snapshot& snap);

 private:
  Cellnta::Snapshot m_prevSnap;
};

class SetMiddlePosition : public Action::Base {
 public:
  void Execute() override;
};

class CreateBlinker : public Action::Pattern::Base {
 public:
  CreateBlinker(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class CreateStair : public Action::Pattern::Base {
 public:
  CreateStair(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class Create1dLine : public Action::Pattern::Base {
 public:
  Create1dLine(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class Create2dGlider : public Action::Pattern::Base {
 public:
  Create2dGlider(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

}  // namespace Pattern
}  // namespace Action
}  // namespace Ui
