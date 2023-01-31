#include "AlgoSimpleSubWindow.h"

#include "Context.h"
#include "Widgets/Eigen.h"
#include "Widgets/Utils.h"

using namespace Ui;

void Ui::DrawWorldImplSimple(Cellnta::WorldImplSimple* world) {
  if (world == nullptr)
    return;

  bool repeated = world->GetWorldRepeated();
  if (ImGui::Checkbox("World repeated", &repeated))
    world->SetWorldRepeated(repeated);

  auto size = world->GetSize();
  if (Widget::DragN("Size", size.data(), size.size()))
    world->SetSize(size);

  Widget::Separator();

  boost::dynamic_bitset<> born = world->GetBorn();
  if (Widget::RuleMask("Born", born))
    world->SetBorn(born);

  boost::dynamic_bitset<> survive = world->GetSurvive();
  if (Widget::RuleMask("Survive", survive))
    world->SetSurvive(survive);
}
