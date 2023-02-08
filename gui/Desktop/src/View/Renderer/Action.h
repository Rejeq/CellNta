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

class SetHypercubeDimension : public Action::Base {
 public:
  SetHypercubeDimension(int dim) : m_dim(dim) {}
  void Execute() override;

 private:
  int m_dim;
};

class SetHypercubeMode : public Action::Base {
 public:
  SetHypercubeMode(const Cellnta::CubeMode& mode) : m_mode(mode) {}
  void Execute() override;

 private:
  Cellnta::CubeMode m_mode;
};

class SetRenderDistance : public Action::Base {
 public:
  SetRenderDistance(int distance) : m_distance(distance) {}
  void Execute() override;

 private:
  int m_distance;
};

class SetCell : public Action::Base {
 public:
  SetCell(const Cellnta::Cell& cell) : m_cell(cell) {}
  void Execute() override;

 private:
  Cellnta::Cell m_cell;
};

}  // namespace Renderer
}  // namespace Action
}  // namespace Ui
