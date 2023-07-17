#include "World/Window.h"

#include <array>

#include <Cellnta/World/Impl/Random.h>

#include "Context.h"
#include "Widgets/ComboEnum.h"
#include "Widgets/Utils.h"
#include "World/Action.h"
#include "World/RandomView.h"
#include "World/RuleAction.h"
#include "World/SimpleView.h"

using namespace Ui;

void WorldWindow::Draw() {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();
  if (ctx == nullptr)
    return;

  Cellnta::World* world = &GetContext()->GetWorld();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    DrawBaseWorldInfo(world);

    Widget::Separator();

    switch (world->GetType()) {
      case Cellnta::WorldType::RANDOM:
        WorldRandomView::Draw(*ctx, *((Cellnta::WorldImplRandom*)world));
        break;
      case Cellnta::WorldType::SIMPLE:
        WorldSimpleView::Draw(*ctx, *((Cellnta::WorldImplSimple*)world));
        break;
      default: break;
    }
  }

  ImGui::End();
}

void WorldWindow::DrawBaseWorldInfo(Cellnta::World*& world) {
  CELLNTA_PROFILE;

  static const std::array<ComboData<Cellnta::WorldType>, 2> WorldTypeData = {
      ComboData(Cellnta::WorldType::RANDOM, "Random"),
      ComboData(Cellnta::WorldType::SIMPLE, "Simple"),
  };

  if (world == nullptr)
    return;

  Context* ctx = GetContext();

  Cellnta::WorldType res = Cellnta::WorldType::COUNT;
  if (Widget::ComboEnum("World type", world->GetType(), WorldTypeData, res)) {
    ctx->PushAction(Action::Make(Action::World::SetWorldType(res)));
    world = &ctx->GetWorld();  // FIXME: Remove this, when used the Action will
                               // be performed at the next frame
  }

  size_t dim = world->GetDimension();
  if (ImGui::DragInt("Dimensions", (int*)&dim, 0.125f, 0, INT_MAX))
    ctx->PushAction(Action::Make(Action::Rule::SetDimension(dim)));

  ImGui::Spacing();

  int32_t step = world->GetStep();
  if (ImGui::DragInt("Step", (int*)&step, 1.0f, 0, INT32_MAX, nullptr,
                     ImGuiSliderFlags_AlwaysClamp))
    ctx->PushAction(Action::Make(Action::World::SetStep(step)));

  if (ImGui::Button("Next generation"))
    ctx->PushAction(Action::Make(Action::World::NextGeneration()));

  ImGui::SameLine();

  bool enabled = m_timer.Enabled();
  if (ImGui::Checkbox("Auto", &enabled)) {
    if (enabled)
      m_timer.Start(1.0f, [&]() { world->Update(); });
    else
      m_timer.Stop();
  }
  if (enabled)
    m_timer.Update(ImGui::GetIO().DeltaTime);

  ImGui::Spacing();

  Widget::CellSelector(world->GetDimension(), m_setCell);

  if (ImGui::Button("Set cell")) {
    ctx->PushAction(Action::Make(Action::World::SetCell(m_setCell)));
  }

  ImGui::Text("Population: %zu", world->GetPopulation());
}
