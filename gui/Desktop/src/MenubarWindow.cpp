#include "MenubarWindow.h"

#include "Context.h"
#include "Widgets.h"

using namespace Ui;

void MenubarWindow::Draw() {
  CELLNTA_PROFILE;

  if (ImGui::BeginMainMenuBar()) {
    DrawWindowsButtons();

    // TODO: Create a class for right aligning
    // This method depends on the previous frame
    // so do not use it in dynamic places
    float right =
        ImGui::GetContentRegionMax().x + ImGui::GetStyle().ItemSpacing.x;
    right = DrawFramerate(right);
    DrawResetLayout(right);

    ImGui::EndMainMenuBar();
  }
}

void MenubarWindow::DrawWindowsButtons() {
  CELLNTA_PROFILE;

  ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
  activeColor.y *= 1.5f;
  activeColor.z *= 0.6f;

  ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
  for (const auto& window : GetContext()->GetWindows()) {
    WindowProperties& prop = window->GetProperties();
    if (prop.VisibleInMenubar)
      Widget::ToggleButton(prop.Name, prop.Opened);
  }
  ImGui::PopStyleColor();
}

float MenubarWindow::DrawFramerate(float right) {
  CELLNTA_PROFILE;

  const ImGuiIO& io = ImGui::GetIO();

  if (m_frameratePos != -1.0f)
    ImGui::SetCursorPosX(m_frameratePos);

  ImGui::Text("FPS: %i(%.3fms)", (int)io.Framerate, io.DeltaTime * 1000);

  float width = ImGui::GetCursorPosX() - m_frameratePos;
  m_frameratePos = right - width;

  return m_frameratePos;
}

float MenubarWindow::DrawResetLayout(float right) {
  CELLNTA_PROFILE;

  const ImGuiIO& io = ImGui::GetIO();

  if (m_layoutPos != -1.0f)
    ImGui::SetCursorPosX(m_layoutPos);

  if (ImGui::Button("Reset layout"))
    if (m_OnResetLayout)
      m_OnResetLayout(*GetContext());

  float width = ImGui::GetCursorPosX() - m_layoutPos;
  m_layoutPos = right - width;

  return m_layoutPos;
}
