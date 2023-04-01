#include "UndoRedo.h"

#include "Log.h"

using namespace Ui;

void UndoRedo::PushAction(Action::BasePtr&& action) {
  if (!action->CanUndo()) {
    DESKTOP_LOG_WARN("Attempting to add an non undoable action to UndoRedo manager. Ignoring");
    return;
  }

  m_undo.push(std::move(action));
  m_redo = std::stack<Action::BasePtr>();
}

void UndoRedo::Undo() {
  if (!CanUndo())
    return;

  Action::BasePtr& action = m_undo.top();
  action->Undo();
  m_redo.push(std::move(action));
  m_undo.pop();
}

void UndoRedo::Redo() {
  if (!CanRedo())
    return;

  Action::BasePtr& action = m_redo.top();
  action->Execute();
  m_undo.push(std::move(action));
  m_redo.pop();
}
