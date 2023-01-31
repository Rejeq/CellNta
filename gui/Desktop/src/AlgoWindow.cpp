#include "AlgoWindow.h"

#include <array>

#include <Cellnta/Renderer/RenderData.h>

#include "AlgoRandomSubWindow.h"
#include "AlgoSimpleSubWindow.h"
#include "Context.h"
#include "Widgets/ComboEnum.h"
#include "Widgets/Utils.h"

using namespace Ui;

void AlgoWindow::Draw() {
  CELLNTA_PROFILE;

  Cellnta::AlgoBase* algo = &GetContext()->GetAlgo();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    DrawBaseAlgoInfo(algo);

    Widget::Separator();

    switch (algo->GetType()) {
      case Cellnta::AlgoType::RANDOM:
        DrawAlgoRandom((Cellnta::AlgoRandom*)algo);
        break;
      case Cellnta::AlgoType::SIMPLE:
        DrawAlgoSimple((Cellnta::AlgoSimple*)algo);
        break;
      default: break;
    }
  }

  ImGui::End();
}

void AlgoWindow::DrawBaseAlgoInfo(Cellnta::AlgoBase*& algo) {
  CELLNTA_PROFILE;

  static const std::array<ComboData<Cellnta::AlgoType>, 2> AlgoTypeData = {
      ComboData(Cellnta::AlgoType::RANDOM, "Random"),
      ComboData(Cellnta::AlgoType::SIMPLE, "Simple"),
  };

  if (algo == nullptr)
    return;

  Context* ctx = GetContext();

  Cellnta::AlgoType res = Cellnta::AlgoType::COUNT;
  if (Widget::ComboEnum("Algorithm type", algo->GetType(), AlgoTypeData, res)) {
    if (ctx->SetAlgo(res))
      assert(0 && "Unable to change algorithm type");
    algo = &ctx->GetAlgo();
  }

  size_t dim = algo->GetDimensions();
  if (ImGui::DragInt("Dimensions", (int*)&dim, 0.125f, 0, INT_MAX))
    algo->SetDimension(dim);

  ImGui::Spacing();

  int32_t step = algo->GetStep();
  if (ImGui::DragInt("Step", (int*)&step, 1.0f, 0, INT32_MAX, nullptr,
                     ImGuiSliderFlags_AlwaysClamp))
    algo->SetStep(step);

  if (ImGui::Button("Next generation"))
    GetContext()->NextGeneration();

  ImGui::SameLine();

  bool enabled = m_timer.Enabled();
  if (ImGui::Checkbox("Auto", &enabled)) {
    if (enabled)
      m_timer.Start(1.0f, [&]() { algo->Update(); });
    else
      m_timer.Stop();
  }
  if (enabled)
    m_timer.Update(ImGui::GetIO().DeltaTime);

  ImGui::Spacing();

  Widget::CellSelector(algo->GetDimensions(), m_setCell);

  if (ImGui::Button("Set cell")) {
    algo->SetCell(m_setCell);
    Cellnta::RenderData* renData = GetContext()->GetRenderer().GetData();
    if (renData != nullptr)
      renData->SetCell(m_setCell);
  }
}
