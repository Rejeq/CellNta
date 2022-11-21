#include "MenubarWindow.h"

#include "Widgets.h"
#include "Context.h"

using namespace Ui;

void MenubarWindow::Draw()
{
  CELLNTA_PROFILE;

  ImGui::BeginMainMenuBar();

  ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
  activeColor.y *= 1.5f;
  activeColor.z *= 0.6f;

  ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
  for (const auto& window : GetContext()->GetWindows())
  {
    WindowProperties& prop = window->GetProperties();
    if(prop.VisibleInMenubar)
      Widget::ToggleButton(prop.Name, prop.Opened);
  }
  ImGui::PopStyleColor();

  const float maxWidth = ImGui::GetContentRegionMax().x;
  float framerateWidth = 0;
  framerateWidth = DrawFramerate(maxWidth);

  //TODO:
  constexpr const char* ResetLayoutStr = "Reset layout";
  float resetLayoutWidth = ImGui::CalcTextSize(ResetLayoutStr).x;

  const float spacing = ImGui::GetStyle().ItemSpacing.x;
  ImGui::SetCursorPosX(maxWidth - framerateWidth - resetLayoutWidth - spacing - 10);
  if (ImGui::Button(ResetLayoutStr))
    if(m_OnResetLayout)
      m_OnResetLayout(*GetContext());

  ImGui::EndMainMenuBar();
}

float MenubarWindow::DrawFramerate(const float& offset)
{
  constexpr const char* FramrateStr = "FPS: %i(%.3fms)";

  const float framerate = ImGui::GetIO().Framerate;
  size_t size = 0;
  char* text = GetContext()->GetTmpBuffer(size);

  snprintf(text, size, FramrateStr, (int)framerate, 1000.0f / framerate);
  const float textWidth = ImGui::CalcTextSize(text).x;

  if (offset > 0)
    ImGui::SetCursorPosX(offset - textWidth);
  ImGui::TextUnformatted(text);
  return textWidth;
}
