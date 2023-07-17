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

  ImGui::Spacing();

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
      m_timer.Start(1.0f, [ctx]() {
        ctx->PushAction(Action::Make(Action::World::NextGeneration()));
      });
    else
      m_timer.Stop();
  }
  if (enabled)
    m_timer.Update(ImGui::GetIO().DeltaTime);

  ImGui::Text("Population: %zu", world->GetPopulation());

  Widget::Separator();

  Widget::CellSelector(world->GetDimension(), m_setCell);

  if (ImGui::Button("Set cell")) {
    ctx->PushAction(Action::Make(Action::World::SetCell(m_setCell)));
  }

  Widget::Separator();

  auto axisSize = world->GetAxisSizeList();
  if (Widget::DragN("Axis lengths", axisSize.data(), axisSize.size()))
    ctx->PushAction(Action::Make(Action::World::SetSize(axisSize)));

  ImGui::Spacing();

  DrawRuleAllStatesSelector(*world);

  ImGui::Spacing();

  DrawRuleNewSpecialStateSelector(*world);
}

void WorldWindow::DrawRuleAllStatesSelector(const Cellnta::World& world) {
  // TODO: String states instead of numbers

  // NOTE: Actions creates new rules, but its not lead to any bugs, because
  // the rule points to the World stack, so address does not change

  Context* ctx = GetContext();

  const Cellnta::Rule& rule = world.GetRule();

  int totalStates = rule.GetTotalStates();
  if (Widget::Input("Count Of States", &totalStates, 1,
                    ImGuiInputTextFlags_CharsDecimal)) {
    if (totalStates >= 1)
      ctx->PushAction(Action::Make(Action::Rule::SetTotalStates(totalStates)));
  }

  for (Cellnta::Cell::State state = 0; state < rule.GetTotalStates(); state++) {
    auto nodeLabel = fmt::format("State: {}", (int)state);
    if (ImGui::TreeNode(nodeLabel.c_str())) {
      DrawRuleStateDataSelector(rule, state);
      ImGui::TreePop();
    }
  }
}

void WorldWindow::DrawRuleStateDataSelector(const Cellnta::Rule& rule,
                                            Cellnta::Cell::State state) {
  Context* ctx = GetContext();

  Cellnta::Cell::State fallback = rule.GetFallbackState(state);
  if (Widget::Input("Fallback", &fallback, 1,
                    ImGuiInputTextFlags_CharsDecimal)) {
    ctx->PushAction(
        Action::Make(Action::Rule::SetFallbackState(state, fallback)));
  }

  auto iter = rule.MakeSpecialStatesIter(state);

  // Iterator are already created, but we need to change internal state of
  // iterator. So just store all changes in tmp vector
  std::vector<Action::BasePtr> changes;

  while (const auto* data = iter.Next()) {
    auto mask = data->mask;
    auto become = data->become;

    ImGui::Spacing();

    if (DrawRuleMaskSelector(rule, mask)) {
      changes.emplace_back(
          Action::Make(Action::Rule::ReplaceState(state, data->mask, mask)));
    }

    if (Widget::Input("Become", &become, 1, ImGuiInputTextFlags_CharsDecimal)) {
      changes.emplace_back(
          Action::Make(Action::Rule::SetState(state, mask, become)));
    }
  }

  for (auto& change : changes)
    ctx->PushAction(std::move(change));
}

void WorldWindow::DrawRuleNewSpecialStateSelector(const Cellnta::World& world) {
  Context* ctx = GetContext();

  const Cellnta::Rule& rule = world.GetRule();

  Widget::Input("State", &m_newState, 1, ImGuiInputTextFlags_CharsDecimal);

  DrawRuleMaskSelector(rule, m_newMask);
  Widget::Input("Become", &m_newBecome, 1, ImGuiInputTextFlags_CharsDecimal);

  if (ImGui::Button("Add")) {
    ctx->PushAction(Action::Make(
        Action::Rule::SetState(m_newState, m_newMask, m_newBecome)));
  }
}

bool WorldWindow::DrawRuleMaskSelector(const Cellnta::Rule& rule,
                                       Cellnta::Rule::Mask& mask) {
  constexpr char Delimeter = ',';
  constexpr size_t StrBufferSize = 1024;

  auto FilterNumList = [](ImGuiInputTextCallbackData* data) -> int {
    auto ch = data->EventChar;

    if (std::isdigit(ch) || std::isspace(ch) || ch == ',')
      return false;
    return true;
  };

  auto ListToString = [](std::vector<size_t> list) -> std::string {
    std::stringstream ss;
    for (size_t i = 0; i < list.size(); i++) {
      if (i != 0)
        ss << Delimeter << " ";
      ss << list[i];
    }
    return ss.str();
  };

  auto StringToList = [&](const std::string& str) -> std::vector<size_t> {
    std::stringstream ss(str);
    std::vector<size_t> list;
    int tmp;

    while (ss >> tmp) {
      list.emplace_back(tmp);

      if (ss.peek() == Delimeter || ss.peek() == ' ')
        ss.ignore();
    }

    return list;
  };

  ImGui::PushID(mask);

  bool updated = false;
  auto quanList = rule.MaskToQuantityList(mask);
  std::string quanStr = ListToString(quanList);
  quanStr.resize(StrBufferSize, '\0');

  if (ImGui::InputText("Mask", quanStr.data(), StrBufferSize,
                       ImGuiInputTextFlags_CallbackCharFilter, FilterNumList)) {
    auto newList = StringToList(quanStr);

    if (newList.empty()) {
      DESKTOP_LOG_ERROR(
          "Unable to create rule mask: Incorrect quantity string '{}'",
          quanStr);
    } else {
      mask = rule.QuantityListToMask(newList);
      updated = true;
    }
  }

  ImGui::PopID();
  return updated;
}
