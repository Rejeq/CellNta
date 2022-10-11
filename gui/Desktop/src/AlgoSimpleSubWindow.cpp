#include "AlgoSimpleSubWindow.h"

#include "Widgets.h"
#include "Context.h"

namespace Ui
{
  void DrawAlgoSimple(Lf::AlgoSimple* algo)
  {
    if (algo == nullptr)
      return;

    bool repeated = algo->GetWorldRepeated();
    if (ImGui::Checkbox("World repeated", &repeated))
      algo->SetWorldRepeated(repeated);

    auto size = algo->GetSize();
    if(Widget::DragN("Size", size.data(), size.size()))
      algo->SetSize(size);

    Widget::Separator();

    boost::dynamic_bitset<> born = algo->GetBorn();
    if (Widget::RuleMask("Born", born))
      algo->SetBorn(born);

    boost::dynamic_bitset<> survive = algo->GetSurvive();
    if (Widget::RuleMask("Survive", survive))
      algo->SetSurvive(survive);
  }
}
