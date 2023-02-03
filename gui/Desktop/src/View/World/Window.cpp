#include "View/World/Window.h"

#include <array>

#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/World/Impl/Simple.h>
#include <Cellnta/World/Impl/Random.h>

#include "Context.h"
#include "Widgets/ComboEnum.h"
#include "Widgets/Utils.h"

using namespace Ui;

static void DrawWorldImplSimple(Cellnta::WorldImplSimple* world) {
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

static void DrawWorldImplRandom(Cellnta::WorldImplRandom* world) {
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

void WorldWindow::Draw() {
  CELLNTA_PROFILE;

  Cellnta::World* world = &GetContext()->GetWorld();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    DrawBaseWorldInfo(world);

    Widget::Separator();

    switch (world->GetType()) {
      case Cellnta::WorldType::RANDOM:
        DrawWorldImplRandom((Cellnta::WorldImplRandom*)world);
        break;
      case Cellnta::WorldType::SIMPLE:
        DrawWorldImplSimple((Cellnta::WorldImplSimple*)world);
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
    if (ctx->SetWorld(res))
      assert(0 && "Unable to change worldrithm type");
    world = &ctx->GetWorld();
  }

  size_t dim = world->GetDimensions();
  if (ImGui::DragInt("Dimensions", (int*)&dim, 0.125f, 0, INT_MAX))
    world->SetDimension(dim);

  ImGui::Spacing();

  int32_t step = world->GetStep();
  if (ImGui::DragInt("Step", (int*)&step, 1.0f, 0, INT32_MAX, nullptr,
                     ImGuiSliderFlags_AlwaysClamp))
    world->SetStep(step);

  if (ImGui::Button("Next generation"))
    GetContext()->NextGeneration();

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
    world->SetCell(m_setCell);
    Cellnta::RenderData* renData = GetContext()->GetRenderer().GetData();
    if (renData != nullptr)
      renData->SetCell(m_setCell);
  }
}
