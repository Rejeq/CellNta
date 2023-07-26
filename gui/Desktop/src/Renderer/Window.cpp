#include "Renderer/Window.h"

#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"
#include "Renderer/HypercubeView.h"
#include "Renderer/RenderDataView.h"
#include "Renderer/ShuffleView.h"
#include "Widgets/Utils.h"

using namespace Ui;

void RendererWindow::Draw() {
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags WinFlags = ImGuiWindowFlags_HorizontalScrollbar;

  Context* ctx = GetContext();
  if (ctx == nullptr)
    return;

  Cellnta::Renderer& ren = ctx->GetRenderer();
  if (ImGui::Begin(p_prop.Name, &p_prop.Opened, WinFlags)) {
    auto* cube = ren.GetHypercube();
    if (cube != nullptr)
      HypercubeView::Draw(*ctx, *cube);

    Widget::Separator();

    auto* data = ren.GetData();
    if (data != nullptr)
      RenderDataView::Draw(*ctx, *data);

    Widget::Separator();
    if (data != nullptr) {
      ShuffleView::DrawWithTreeNode(*ctx, data->GetCells());
    }
  }

  ImGui::End();
}
