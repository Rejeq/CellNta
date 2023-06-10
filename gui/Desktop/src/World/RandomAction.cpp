#include "World/RandomAction.h"

#include <Cellnta/World/Impl/Random.h>

#include "Context.h"
#include "Log.h"
#include "Renderer/RenderDataAction.h"

using namespace Ui;
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
