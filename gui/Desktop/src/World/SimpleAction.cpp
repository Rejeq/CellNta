#include "World/SimpleAction.h"

#include <Cellnta/World/Impl/Simple.h>

#include "Context.h"
#include "Log.h"
#include "Renderer/RenderDataAction.h"

using namespace Ui;
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
