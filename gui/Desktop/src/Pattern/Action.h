#pragma once

#include <Cellnta/Cell.h>
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
  SetMiddlePosition(Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos& m_pos;
};

class CreateBlinker : public Action::Pattern::Base {
 public:
  CreateBlinker(const Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

class CreateStair : public Action::Pattern::Base {
 public:
  CreateStair(const Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

class Create1dLine : public Action::Pattern::Base {
 public:
  Create1dLine(const Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

class Create2dGlider : public Action::Pattern::Base {
 public:
  Create2dGlider(const Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

class Create2dDiod : public Action::Pattern::Base {
 public:
  Create2dDiod(const Cellnta::Cell::Pos& pos) : m_pos(pos) {}
  void Execute() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

}  // namespace Pattern
}  // namespace Action
}  // namespace Ui
