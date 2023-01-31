#include "AlgoRandomSubWindow.h"
#include "Context.h"

using namespace Ui;

void Ui::DrawWorldImplRandom(Cellnta::WorldImplRandom* world) {
  if (world == nullptr)
    return;

  int min = world->GetRangeMin();
  int max = world->GetRangeMax();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Min", &min))
    world->SetRangeMin(min);

  ImGui::SameLine();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Max", &max))
    world->SetRangeMax(max);

  ImGui::Spacing();

  static int seed = 0;
  if (ImGui::InputInt("Seed", &seed))
    world->SetSeed(seed);
}
