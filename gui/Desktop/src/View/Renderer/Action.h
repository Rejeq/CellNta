#pragma once

#include <Cellnta/Renderer/HypercubeStorage.h>
#include <Cellnta/Renderer/RenderData.h>

#include "ActionBase.h"

namespace Ui {
namespace Action {
namespace Renderer {

class Update : public Action::Base {
 public:
  void Execute() override;
};

class SetHypercubeDimension : public Action::UndoBase {
 public:
  SetHypercubeDimension(int dim) : m_dim(dim) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_dim;
  int m_prevDim;
};

class SetHypercubeMode : public Action::UndoBase {
 public:
  SetHypercubeMode(const Cellnta::CubeMode& mode) : m_mode(mode) {}
  void Execute() override;
  void Undo() override;

 private:
  Cellnta::CubeMode m_mode;
  Cellnta::CubeMode m_prevMode = Cellnta::CubeMode::NONE;
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

}  // namespace Renderer
}  // namespace Action
}  // namespace Ui
