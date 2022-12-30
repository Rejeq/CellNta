#include "AlgoSimpleSubWindow.h"

#include "Context.h"
#include "Widgets/Eigen.h"
#include "Widgets/Utils.h"

using namespace Ui;

void Ui::DrawAlgoSimple(Cellnta::AlgoSimple* algo) {
  if (algo == nullptr)
    return;

  bool repeated = algo->GetWorldRepeated();
  if (ImGui::Checkbox("World repeated", &repeated))
    algo->SetWorldRepeated(repeated);

  auto size = algo->GetSize();
  if (Widget::DragN("Size", size.data(), size.size()))
    algo->SetSize(size);

  Widget::Separator();

  boost::dynamic_bitset<> born = algo->GetBorn();
  if (Widget::RuleMask("Born", born))
    algo->SetBorn(born);

  boost::dynamic_bitset<> survive = algo->GetSurvive();
  if (Widget::RuleMask("Survive", survive))
    algo->SetSurvive(survive);
}
