#pragma once

#include <Cellnta/Renderer/RenderData.h>

#include "ActionBase.h"

namespace Ui::Action::Renderer {

class Update : public Action::Base {
 public:
  void Execute() override;
};

class SetRenderDistance : public Action::UndoBase {
 public:
  SetRenderDistance(int distance) : m_distance(distance) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_distance;
  int m_prevDistance = -1;
};

class SetCell : public Action::UndoBase {
 public:
  SetCell(const Cellnta::Cell& cell) : m_cell(cell) {}
  void Execute() override;
  void Undo() override;

 private:
  Cellnta::Cell m_cell;
  // Cellnta::Cell::State m_prevState;
};

}  // namespace Ui::Action::Renderer
