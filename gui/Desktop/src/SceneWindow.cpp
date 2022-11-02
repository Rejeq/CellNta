#include "SceneWindow.h"

#include "Widgets.h"
#include "Context.h"

namespace Ui
{
  void SceneWindow::Draw()
  {
    ProfileScope;

    const ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoBringToFrontOnFocus
      | ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin(p_prop.Name, nullptr, winFlags))
    {
      GetContext()->SceneWindowFocused = ImGui::IsWindowFocused();

      ImVec2 size = ImGui::GetContentRegionAvail();

      //TODO: add owerflow operator for ImVec*
      if (size.x != m_prevWindowSize.x || size.y != m_prevWindowSize.y)
        GetContext()->GetCanvas().OnResize(size.x, size.y);

      ImGui::Image((void*)GetContext()->SceneTextureId, size,
        ImVec2(0, 1), ImVec2(1, 0));

      m_prevWindowSize = size;
    }
    ImGui::PopStyleVar(3);
    ImGui::End();
  }
}
