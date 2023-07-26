#include "Renderer/ShuffleView.h"

#include "Renderer/ShuffleAction.h"

using namespace Ui;

void ShuffleView::Draw(Context& ctx, Cellnta::ShuffleAxisStorage& shuffle) {
  bool shuffleOnDraw = shuffle.NeedShuffleOnDraw();
  if (ImGui::Checkbox("Shuffle On Draw", &shuffleOnDraw))
    ctx.PushAction<Action::Shuffle::SetShuffleOnDraw>(shuffleOnDraw);

  for (int axis = 0; axis < shuffle.GetDimension(); axis++) {
    auto shuffleAxis = shuffle.GetShuffleAxis(axis);

    ImGui::PushID(axis);

    ImGui::Text("%d =", axis);
    ImGui::SameLine();
    if (ImGui::InputInt("##WithAxis", &shuffleAxis))
      ctx.PushAction<Action::Shuffle::SetAxis>(axis, shuffleAxis);
    ImGui::Spacing();

    ImGui::PopID();
  }
}

void ShuffleView::DrawWithTreeNode(Context& ctx,
                                   Cellnta::ShuffleAxisStorage& shuffle,
                                   ImGuiTreeNodeFlags flags) {
  if (ImGui::TreeNodeEx("Shuffle axes", flags)) {
    Draw(ctx, shuffle);
    ImGui::TreePop();
  }
}
