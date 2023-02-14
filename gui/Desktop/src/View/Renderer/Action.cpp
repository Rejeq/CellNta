#include "View/Renderer/Action.h"

#include <Cellnta/Renderer/Renderer.h>
#include <Cellnta/World/World.h>

#include "Context.h"
#include "Log.h"

using namespace Ui::Action::Renderer;

void Update::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if(Action::CheckLogDErr(!data, "Renderer::Update"))
    return;

  data->Update(&p_ctx->GetWorld());
}

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
  if(Action::CheckLogDErr(!cube, "Renderer::SetHypercubeMode"))
    return;

  cube->SetMode(m_mode);
}

void SetCell::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* data = ren.GetData();
  if(Action::CheckLogDErr(!data, "Renderer::SetCell"))
    return;

  data->SetCell(m_cell);
}
