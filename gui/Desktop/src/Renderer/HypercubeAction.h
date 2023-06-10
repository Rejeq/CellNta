#pragma once

#include <Cellnta/Renderer/HypercubeStorage.h>

#include "ActionBase.h"

namespace Ui::Action::Renderer {

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

}  // namespace Ui::Action::Renderer

