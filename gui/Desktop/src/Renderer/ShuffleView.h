#pragma once

#include <imgui.h>

#include <Cellnta/Renderer/ShuffleAxisStorage.h>

#include "Context.h"

namespace Ui {

class ShuffleView {
 public:
  static void Draw(Context& ctx, Cellnta::ShuffleAxisStorage& shuffle);
  static void DrawWithTreeNode(Context& ctx,
                               Cellnta::ShuffleAxisStorage& shuffle,
                               ImGuiTreeNodeFlags flags = 0);
};

}  // namespace Ui
