#pragma once

#include <stack>

#include "ActionBase.h"

namespace Ui {

class UndoRedo {
 public:
  void PushAction(Action::BasePtr&& command);

  void Undo();
  int UndoSize() const { return m_undo.size(); }
  bool CanUndo() const { return !m_undo.empty(); }

  void Redo();
  int RedoSize() const { return m_redo.size(); }
  bool CanRedo() const { return !m_redo.empty(); }

 private:
  std::stack<Action::BasePtr> m_undo;
  std::stack<Action::BasePtr> m_redo;
};

}  // namespace Ui
