#include "View/World/Action.h"

#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/Renderer/Renderer.h>
#include <Cellnta/World/WorldList.h>

#include "Context.h"
#include "Log.h"

using namespace Ui;
using namespace Ui::Action::World;

void NextGeneration::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();

  world.Update();

  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* renData = ren.GetData();
  if(Action::CheckLogWarn(!renData, "World::NextGeneration action"))
    return;

  renData->Update(&world);
}

void SetWorldType::Execute() {
  if (p_ctx->SetWorld(m_type))
    DESKTOP_LOG_ERROR(DESKTOP_ACTION_DERR_MSG "World::SetWorldType");
}

void SetDimension::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  world.SetDimension(m_dim);
}

void SetStep::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  world.SetStep(m_step);
}

void SetCell::Execute() {
  Cellnta::World& world = p_ctx->GetWorld();
  world.SetCell(m_cell);

  Cellnta::Renderer& ren = p_ctx->GetRenderer();
  Cellnta::RenderData* renData = ren.GetData();
  if(Action::CheckLogWarn(!renData, "Unable to set RenderData cell in World::SetCell action"))
    return;

  renData->SetCell(m_cell);
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
  if(Action::CheckLogDErr(!world, "SimpleWorld::SetRepeated"))
    return;

  world->SetWorldRepeated(m_repeated);
}

void SetSize::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if(Action::CheckLogDErr(!world, "SimpleWorld::SetSize"))
    return;

  world->SetSize(m_size);
}

void SetBorn::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if(Action::CheckLogDErr(!world, "SimpleWorld::SetBorn"))
    return;

  world->SetBorn(m_born);
}

void SetSurvive::Execute() {
  Cellnta::WorldImplSimple* world = GetSimpleWorld(p_ctx->GetWorld());
  if(Action::CheckLogDErr(!world, "SimpleWorld::SetSurvive"))
    return;

  world->SetSurvive(m_survive);
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
  if(Action::CheckLogDErr(!world, "RandomWorld::SetRangeMin"))
    return;

  world->SetRangeMin(m_min);
}

void SetRangeMax::Execute() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if(Action::CheckLogDErr(!world, "RandomWorld::SetRangeMax"))
    return;

  world->SetRangeMax(m_max);
}

void SetSeed::Execute() {
  Cellnta::WorldImplRandom* world = GetRandomWorld(p_ctx->GetWorld());
  if(Action::CheckLogDErr(!world, "RandomWorld::SetSeed"))
    return;

  world->SetSeed(m_seed);
}
