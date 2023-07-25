#pragma once

#include "ActionBase.h"

namespace Ui::Action::Renderer {

class SetIgnoreCubeProject : public Action::UndoBase {
 public:
  SetIgnoreCubeProject(bool state) : m_state(state) {}
  void Execute() override;
  void Undo() override;

 private:
  bool m_state;
  bool m_prevState;
};

class SetIgnoreCellProject : public Action::UndoBase {
 public:
  SetIgnoreCellProject(bool state) : m_state(state) {}
  void Execute() override;
  void Undo() override;

 private:
  bool m_state;
  bool m_prevState;
};

}  // namespace Ui::Action::Renderer
