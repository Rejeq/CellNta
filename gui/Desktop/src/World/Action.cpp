#include "World/Action.h"

#include "Context.h"
#include "Log.h"
#include "Renderer/RenderDataAction.h"

using namespace Ui;
using namespace Ui::Action::World;

void NextGeneration::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();

  world.Update();

  p_ctx->PushAction(Action::Make(Action::Renderer::Update()));
}

void SetWorldType::Execute() {
  m_prevType = p_ctx->GetWorld().GetType();
  if (p_ctx->SetWorld(m_type))
    DESKTOP_LOG_ERROR(DESKTOP_ACTION_DERR_MSG "World::SetWorldType");
}

void SetWorldType::Undo() {
  if (p_ctx->SetWorld(m_prevType))
    DESKTOP_LOG_ERROR(DESKTOP_ACTION_DERR_MSG "World::SetWorldType");
}

void SetDimension::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  m_prevDim = world.GetDimension();
  world.SetDimension(m_dim);
}

void SetDimension::Undo() {
  Cellnta::World& world = p_ctx->GetWorld();
  world.SetDimension(m_prevDim);
}

void SetStep::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  m_prevStep = world.GetStep();
  world.SetStep(m_step);
}

void SetStep::Undo() {
  Cellnta::World& world = p_ctx->GetWorld();
  world.SetStep(m_prevStep);
}

void SetCell::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  m_prevState = world.GetCell(m_cell.pos);
  world.SetCell(m_cell);

  // m_renAction is executed this way because the undo command is executed twice
  // through Context()->PushAction
  if (!m_renAction) {
    m_renAction = Action::Make(Action::Renderer::SetCell(m_cell));
    m_renAction->SetContext(p_ctx);
  }

  m_renAction->Execute();
}

void SetCell::Undo() {
  Cellnta::World& world = p_ctx->GetWorld();

  world.SetCell(m_cell.pos, m_prevState);
  if (m_renAction)
    m_renAction->Undo();
}
