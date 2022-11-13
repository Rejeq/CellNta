#include "AlgoRandomSubWindow.h"
#include "Context.h"

using namespace Ui;

void Ui::DrawAlgoRandom(Cellnta::AlgoRandom* algo)
{
  if (algo == nullptr)
    return;

  int min = algo->GetRangeMin();
  int max = algo->GetRangeMax();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if(ImGui::DragInt("Min", &min))
    algo->SetRangeMin(min);

  ImGui::SameLine();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Max", &max))
    algo->SetRangeMax(max);

  ImGui::Spacing();

  static int seed = 0;
  if (ImGui::InputInt("Seed", &seed))
    algo->SetSeed(seed);
}
