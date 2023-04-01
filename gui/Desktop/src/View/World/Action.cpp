#include "View/World/Action.h"

#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/Renderer/Renderer.h>
#include <Cellnta/World/WorldList.h>

#include "Context.h"
#include "Log.h"
#include "View/Renderer/Action.h"

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

using namespace Ui::Action::SimpleWorld;

static Cellnta::WorldImplSimple* GetSimpleWorld(Cellnta::World& world) {
  if (world.GetType() != Cellnta::WorldType::SIMPLE) {
    DESKTOP_LOG_WARN("WorldType is not a SIMPLE");
    return nullptr;
  }

  return (Cellnta::WorldImplSimple*)&world;
}

void SetRepeated::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetRepeated"))
    return;

  m_prevRepeated = world->GetWorldRepeated();
  world->SetWorldRepeated(m_repeated);
}

void SetRepeated::Undo() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetRepeated"))
    return;

  world->SetWorldRepeated(m_prevRepeated);
}

void SetSize::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetSize"))
    return;

  m_prevSize = world->GetSize();
  world->SetSize(m_size);
}

void SetSize::Undo() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetSize"))
    return;

  world->SetSize(m_prevSize);
}

void SetBorn::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetBorn"))
    return;

  m_prevBorn = world->GetBorn();
  world->SetBorn(m_born);
}

void SetBorn::Undo() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetBorn"))
    return;

  world->SetBorn(m_prevBorn);
}

void SetSurvive::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetSurvive"))
    return;

  m_prevSurvive = world->GetSurvive();
  world->SetSurvive(m_survive);
}

void SetSurvive::Undo() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "SimpleWorld::SetSurvive"))
    return;

  world->SetSurvive(m_prevSurvive);
}

using namespace Ui::Action::RandomWorld;

static Cellnta::WorldImplRandom* GetRandomWorld(Cellnta::World& world) {
  if (world.GetType() != Cellnta::WorldType::RANDOM) {
    DESKTOP_LOG_WARN("WorldType is not a RANOM");
    return nullptr;
  }

  return (Cellnta::WorldImplRandom*)&world;
}

void SetRangeMin::Execute() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetRangeMin"))
    return;

  m_prevMin = world->GetRangeMin();
  world->SetRangeMin(m_min);
}

void SetRangeMin::Undo() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetRangeMin"))
    return;

  world->SetRangeMin(m_prevMin);
}

void SetRangeMax::Execute() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetRangeMax"))
    return;

  m_prevMax = world->GetRangeMax();
  world->SetRangeMax(m_max);
}

void SetRangeMax::Undo() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetRangeMax"))
    return;

  world->SetRangeMax(m_prevMax);
}

void SetSeed::Execute() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetSeed"))
    return;

  m_prevSeed = world->GetSeed();
  world->SetSeed(m_seed);
}

void SetSeed::Undo() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if (Action::CheckLogDErr(!world, "RandomWorld::SetSeed"))
    return;

  world->SetSeed(m_prevSeed);
}
