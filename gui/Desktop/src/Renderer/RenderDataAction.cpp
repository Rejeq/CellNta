#include "Renderer/RenderDataAction.h"

#include "Context.h"
#include "Log.h"

using namespace Ui::Action::Renderer;

void Update::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if (Action::CheckLogDErr(!data, "Renderer::Update"))
    return;

  data->UpdateVisible(p_ctx->GetWorld());
}

void SetRenderDistance::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* renData = ren.GetData();
  if (!Action::CheckLogErr(!renData,
                           "Unable to save prev state for undo in "
                           "Renderer::SetRenderDistance action"))
    m_prevDistance = renData->GetDistance();
  ren.SetRenderDistance(m_distance);
}

void SetRenderDistance::Undo() {
  if (m_prevDistance < 0)
    return;
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  ren.SetRenderDistance(m_prevDistance);
}

void SetCell::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if (Action::CheckLogDErr(!data, "Renderer::SetCell"))
    return;

  // TODO: Don't implemented because NCellStorage not contain
  // cell, only positions
  // m_prevState = data->GetCell(m_cell.pos);
  data->SetCell(m_cell);
}

void SetCell::Undo() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if (Action::CheckLogDErr(!data, "Renderer::SetCell"))
    return;

  // TODO: use m_prevState
  data->SetCell(Cellnta::Cell(m_cell.pos, 0));
}
