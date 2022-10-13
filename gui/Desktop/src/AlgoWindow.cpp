#include "AlgoWindow.h"

#include <array>

#include "Widgets.h"
#include "AlgoRandomSubWindow.h"
#include "AlgoSimpleSubWindow.h"
#include "Context.h"

namespace Ui
{
  void AlgoWindow::Draw()
  {
    ProfileScope;

    Canvas& canvas = GetContext()->GetCanvas();
    Lf::AlgoBase* algo = canvas.GetAlgo();
    if (algo == nullptr)
      return;

    ImGui::Begin(p_name, &p_open);

    DrawBaseAlgoInfo(algo);

    Widget::Separator();

    switch (algo->GetType())
    {
    case Lf::AlgoType::RANDOM: DrawAlgoRandom((Lf::AlgoRandom*)algo); break;
    case Lf::AlgoType::SIMPLE: DrawAlgoSimple((Lf::AlgoSimple*)algo); break;
    default: break;
    }

    ImGui::End();
  }
  void AlgoWindow::DrawBaseAlgoInfo(Lf::AlgoBase* algo)
  {
    ProfileScope;

    static const std::array<ComboData<Lf::AlgoType>, 2> AlgoTypeData = {
      ComboData(Lf::AlgoType::RANDOM, "Random"),
      ComboData(Lf::AlgoType::SIMPLE, "Simple"),
    };

    if (algo == nullptr)
      return;

    Canvas& canvas = GetContext()->GetCanvas();

    Lf::AlgoType res;
    if (Widget::ComboEnum("Algorithm type", algo->GetType(), AlgoTypeData, res))
    {
      if (!canvas.ChangeAlgoType(res))
        algo = canvas.GetAlgo();
      else assert(0 && "Unable to change algorithm type");
    }

    size_t dim = algo->GetDimensions();
    if (ImGui::DragInt("Dimensions", (int*)&dim, 0.125f, 0, INT_MAX))
      algo->SetDimension(dim);

    ImGui::Spacing();

    int32_t step = algo->GetStep();
    if (ImGui::DragInt("Step", (int*)&step, 1.0f, 0, INT32_MAX, nullptr, ImGuiSliderFlags_AlwaysClamp))
      algo->SetStep(step);

    if (ImGui::Button("Next generation"))
      algo->Update();

    ImGui::SameLine();

    bool timer = canvas.TimerEnabled();
    if (ImGui::Checkbox("Auto", &timer))
    {
      if (timer)
        canvas.SetTimer(1.0f);
      else canvas.DisableTimer();
    }

    ImGui::Spacing();

    Widget::CellSelector(algo->GetDimensions(), m_setCell);

    if (ImGui::Button("Set cell"))
      algo->SetCell(m_setCell);
  }
}
