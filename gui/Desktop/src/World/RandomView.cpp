#include "World/RandomView.h"

#include <Cellnta/World/Impl/Random.h>

#include "World/RandomAction.h"

using namespace Ui;

void WorldRandomView::Draw(Context& ctx, Cellnta::WorldImplRandom& world) {
  CELLNTA_PROFILE;

  int seed = world.GetSeed();
  if (ImGui::InputInt("Seed", &seed))
    ctx.PushAction(Action::Make(Action::RandomWorld::SetSeed(seed)));
}
