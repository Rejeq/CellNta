#include "Renderer/HypercubeAction.h"

#include "Context.h"
#include "Log.h"

using namespace Ui::Action::Renderer;

void SetHypercubeDimension::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  m_prevDim = ren.GetDimensions();
  ren.SetDimension(m_dim);
}

void SetHypercubeDimension::Undo() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  ren.SetDimension(m_prevDim);
}

void SetHypercubeMode::Execute() {
  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::HypercubeStorage* cube = ren.GetHypercube();
  if (Action::CheckLogDErr(!cube, "Renderer::SetHypercubeMode"))
    return;

  m_prevMode = cube->GetMode();
  cube->SetMode(m_mode);
}

void SetHypercubeMode::Undo() {
  if (m_prevMode == Cellnta::CubeMode::NONE)
    return;

  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::HypercubeStorage* cube = ren.GetHypercube();
  if (Action::CheckLogDErr(!cube, "Renderer::SetHypercubeMode"))
    return;

  cube->SetMode(m_prevMode);
}
