#pragma once

#include <vector>

#include <Cellnta/Renderer/ShuffleAxisStorage.h>

#include "ActionBase.h"

namespace Ui::Action::Shuffle {

class SetAxis : public Action::UndoBase {
  using Axis = Cellnta::ShuffleAxisStorage::Axis;

 public:
  SetAxis(Axis axis, Axis with) : m_axis(axis), m_with(with) {}
  void Execute() override;
  void Undo() override;

 private:
  Axis m_axis;
  Axis m_with;
  Axis m_prevWith;
};

class SetShuffleOnDraw : public Action::UndoBase {
  using Axis = Cellnta::ShuffleAxisStorage::Axis;

 public:
  SetShuffleOnDraw(bool state) : m_state(state) {}
  void Execute() override;
  void Undo() override;

 private:
  Axis m_state;
  Axis m_prevState;
};

class Reset : public Action::UndoBase {
  using Axis = Cellnta::ShuffleAxisStorage::Axis;

 public:
  Reset() = default;
  void Execute() override;
  void Undo() override;

 private:
  std::vector<Axis> m_prev;
};

}  // namespace Ui::Action::Shuffle
