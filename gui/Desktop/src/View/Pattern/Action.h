#pragma once

#include <Cellnta/Area.h>
#include <Cellnta/World/WorldList.h>

#include "ActionBase.h"

namespace Ui {
namespace Action {
namespace Pattern {

class SetMiddlePosition : public Action::Base {
 public:
  void Execute() override;
};

class CreateBlinker : public Action::Base {
 public:
  CreateBlinker(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class CreateStair : public Action::Base {
 public:
  CreateStair(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class Create1dLine : public Action::Base {
 public:
  Create1dLine(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

class Create2dGlider : public Action::Base {
 public:
  Create2dGlider(const Eigen::VectorXi& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Eigen::VectorXi m_pos;
};

}  // namespace World
}  // namespace Action
}  // namespace Ui
