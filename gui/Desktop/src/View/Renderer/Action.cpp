#include "View/Renderer/Action.h"

#include <Cellnta/Log.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"

using namespace Ui::Action::Renderer;

void SetHypercubeDimension::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  ren.SetDimension(m_dim);
}

void SetRenderDistance::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  ren.SetRenderDistance(m_distance);
}

void SetHypercubeMode::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::HypercubeStorage* cube = ren.GetHypercube();

  if (cube == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute SetHypercubeMode action");
    return;
  }

  cube->SetMode(m_mode);
}

void SetCell::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();

  if (data == nullptr) {
    CELLNTA_LOG_ERROR("Unable to determine RenderData");
    return;
  }

  data->SetCell(m_cell);
}
