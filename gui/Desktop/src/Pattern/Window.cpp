#include "Pattern/Window.h"

#include <imgui.h>

#include "Context.h"
#include "Pattern/Action.h"
#include "Widgets/Utils.h"

using namespace Ui;

void PatternWindow::Draw() {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    Widget::PositionSelector(ctx->GetWorld().GetDimension(), m_pos);
    if (ImGui::Button("Reset to middle")) {
      Action::Pattern::SetMiddlePosition(m_pos).SetContext(ctx).Execute();
    }

    if (ImGui::Button("Blinker")) {
      ctx->PushAction(Action::Make(Action::Pattern::CreateBlinker(m_pos)));
    }

    ImGui::SameLine();
    if (ImGui::Button("Stair")) {
      ctx->PushAction(Action::Make(Action::Pattern::CreateStair(m_pos)));
    }

    ImGui::SameLine();
    if (ImGui::Button("1D Line")) {
      ctx->PushAction(Action::Make(Action::Pattern::Create1dLine(m_pos)));
    }

    ImGui::SameLine();
    if (ImGui::Button("2D Glider")) {
      ctx->PushAction(Action::Make(Action::Pattern::Create2dGlider(m_pos)));
    }
  }

  ImGui::End();
}
