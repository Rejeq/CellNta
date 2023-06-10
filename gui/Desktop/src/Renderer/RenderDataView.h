#pragma once

#include <Cellnta/Renderer/RenderData.h>

#include "Context.h"

namespace Ui {

class RenderDataView {
 public:
  void Draw(Context& ctx, Cellnta::RenderData& data);

 private:
  void DrawLoadedCells(const Cellnta::NCellStorage&);

  Cellnta::Cell m_selectedCell;
};

}  // namespace Ui
