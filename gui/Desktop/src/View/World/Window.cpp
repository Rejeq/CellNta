#include "View/World/Window.h"

#include <array>

#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/World/Impl/Random.h>
#include <Cellnta/World/Impl/Simple.h>

#include "Context.h"
#include "View/World/Action.h"
#include "Widgets/ComboEnum.h"
#include "Widgets/Utils.h"

using namespace Ui;

static void DrawWorldImplSimple(Cellnta::WorldImplSimple* world, Context* ctx) {
  if (world == nullptr || ctx == nullptr)
    return;

  bool repeated = world->GetWorldRepeated();
  if (ImGui::Checkbox("World repeated", &repeated))
    ctx->PushAction(Action::Make(Action::SimpleWorld::SetRepeated(repeated)));

  auto size = world->GetSize();
  if (Widget::DragN("Size", size.data(), size.size()))
    ctx->PushAction(Action::Make(Action::SimpleWorld::SetSize(size)));

  Widget::Separator();

  boost::dynamic_bitset<> born = world->GetBorn();
  if (Widget::RuleMask("Born", born))
    ctx->PushAction(Action::Make(Action::SimpleWorld::SetBorn(born)));

  boost::dynamic_bitset<> survive = world->GetSurvive();
  if (Widget::RuleMask("Survive", survive))
    ctx->PushAction(Action::Make(Action::SimpleWorld::SetSurvive(survive)));
}

static void DrawWorldImplRandom(Cellnta::WorldImplRandom* world, Context* ctx) {
  if (world == nullptr || ctx == nullptr)
    return;

  int min = world->GetRangeMin();
  int max = world->GetRangeMax();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Min", &min))
    ctx->PushAction(Action::Make(Action::RandomWorld::SetRangeMin(min)));

  ImGui::SameLine();

  ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 3);
  if (ImGui::DragInt("Max", &max))
    ctx->PushAction(Action::Make(Action::RandomWorld::SetRangeMax(max)));

  ImGui::Spacing();

  static int seed = 0;
  if (ImGui::InputInt("Seed", &seed))
    ctx->PushAction(Action::Make(Action::RandomWorld::SetSeed(seed)));
}

void WorldWindow::Draw() {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();
  Cellnta::World* world = &GetContext()->GetWorld();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    DrawBaseWorldInfo(world);

    Widget::Separator();

    switch (world->GetType()) {
      case Cellnta::WorldType::RANDOM:
        DrawWorldImplRandom((Cellnta::WorldImplRandom*)world, ctx);
        break;
      case Cellnta::WorldType::SIMPLE:
        DrawWorldImplSimple((Cellnta::WorldImplSimple*)world, ctx);
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

  size_t dim = world->GetDimensions();
  if (ImGui::DragInt("Dimensions", (int*)&dim, 0.125f, 0, INT_MAX))
    ctx->PushAction(Action::Make(Action::World::SetDimension(dim)));

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

  Widget::CellSelector(world->GetDimensions(), m_setCell);

  if (ImGui::Button("Set cell")) {
    ctx->PushAction(Action::Make(Action::World::SetCell(m_setCell)));
  }
}
