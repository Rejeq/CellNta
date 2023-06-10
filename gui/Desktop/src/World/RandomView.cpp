#include "World/RandomView.h"

#include <Cellnta/World/Impl/Random.h>

#include "World/RandomAction.h"

using namespace Ui;

void WorldRandomView::Draw(Context& ctx, Cellnta::WorldImplRandom& world) {
  CELLNTA_PROFILE;

  int min = world.GetRangeMin();
  int max = world.GetRangeMax();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Min", &min))
    ctx.PushAction(Action::Make(Action::RandomWorld::SetRangeMin(min)));

  ImGui::SameLine();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Max", &max))
    ctx.PushAction(Action::Make(Action::RandomWorld::SetRangeMax(max)));

  ImGui::Spacing();

  int seed = world.GetSeed();
  if (ImGui::InputInt("Seed", &seed))
    ctx.PushAction(Action::Make(Action::RandomWorld::SetSeed(seed)));
}
