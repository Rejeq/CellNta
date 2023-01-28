#include "MenubarWindow.h"

#include "Context.h"
#include "Widgets/Utils.h"

using namespace Ui;

void MenubarWindow::Draw() {
  CELLNTA_PROFILE;

  if (ImGui::BeginMainMenuBar()) {
    DrawWindowsButtons();

    m_rAlign.Begin(ImGui::GetContentRegionMax().x +
                   ImGui::GetStyle().ItemSpacing.x);

    const ImGuiIO& io = ImGui::GetIO();
    ImGui::Text("FPS: %i(%.3fms)", (int)io.Framerate, io.DeltaTime * 1000);
    m_rAlign.Next();

    if (ImGui::Button("Reset layout"))
      if (m_OnResetLayout)
        m_OnResetLayout(*GetContext());
    m_rAlign.End();

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
