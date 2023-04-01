#include "View/Menubar/Window.h"

#include "Context.h"
#include "View/Menubar/Action.h"
#include "Widgets/Utils.h"

using namespace Ui;

void MenubarWindow::Draw() {
  CELLNTA_PROFILE;

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      DrawFileMenu();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit")) {
      DrawEditMenu();
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      DrawViewMenu();
      ImGui::EndMenu();
    }

    if (m_showFramerate) {
      m_rAlign.Begin(ImGui::GetContentRegionMax().x +
                     ImGui::GetStyle().ItemSpacing.x);

      const ImGuiIO& io = ImGui::GetIO();
      ImGui::Text("FPS: %i(%.3fms)", (int)io.Framerate, io.DeltaTime * 1000);
      m_rAlign.End();
    }


    ImGui::EndMainMenuBar();
  }
}

void MenubarWindow::DrawFileMenu() {
  bool opened = false;
  ImGui::MenuItem("Open", "Ctrl+O", &opened, false);
  ImGui::MenuItem("Save", "Ctrl+S", &opened, false);
  ImGui::MenuItem("Save As", "Ctrl+Shift-S", &opened, false);

  ImGui::Separator();
  ImGui::MenuItem("Auto Save", nullptr, &opened, false);
  if (ImGui::BeginMenu("Preferences")) {
    ImGui::MenuItem("Keyboard Shortcut", "Ctrl+K Ctrl+S", &opened, false);
    ImGui::MenuItem("Color theme", "Ctrl+K Ctrl+T", &opened, false);

    ImGui::EndMenu();
  }

  ImGui::Separator();
  ImGui::MenuItem("Revert File", nullptr, &opened, false);

  ImGui::Separator();
  ImGui::MenuItem("Exit", nullptr, &opened, false);
}

void MenubarWindow::DrawEditMenu() {
  bool opened = false;
  auto& undoRedo = GetContext()->GetUndoRedo();

  if (ImGui::MenuItem("Undo", "Ctrl+Z", &opened, undoRedo.CanUndo()))
    undoRedo.Undo();
  if (ImGui::MenuItem("Redo", "Ctrl+Y", &opened, undoRedo.CanRedo()))
    undoRedo.Redo();


  ImGui::Separator();
  ImGui::MenuItem("Cut", "Ctrl+X", &opened, false);
  ImGui::MenuItem("Copy", "Ctrl+C", &opened, false);
  ImGui::MenuItem("Paste", "Ctrl+V", &opened, false);
}

void MenubarWindow::DrawViewMenu() {
  bool opened = false;
  DrawWindowsList();

  ImGui::Separator();
  // Appearance
  if (ImGui::MenuItem("Full Screen", "F11"))
    GetContext()->PushAction(Action::Make(Action::Appearance::ToggleFullscreen()));
  if (ImGui::MenuItem("Zoom in", "Ctrl+=", &opened))
    GetContext()->PushAction(Action::Make(Action::Appearance::ZoomIn()));
  if (ImGui::MenuItem("Zoom out", "Ctrl+-", &opened))
    GetContext()->PushAction(Action::Make(Action::Appearance::ZoomOut()));
  if (ImGui::MenuItem("Reset Zoom", nullptr, &opened))
    GetContext()->PushAction(Action::Make(Action::Appearance::ResetZoom()));

  ImGui::Separator();
  ImGui::MenuItem("Toggle All Windows", "F10");

  if (ImGui::MenuItem("ResetLayout")) {
    if (m_OnResetLayout)
      m_OnResetLayout(*GetContext());
  }
  ImGui::MenuItem("Show framerate", nullptr, &m_showFramerate);
}

void MenubarWindow::DrawWindowsList() {
  CELLNTA_PROFILE;

  for (const auto& window : GetContext()->GetWindows()) {
    WindowProperties& prop = window->GetProperties();
    if (prop.VisibleInMenubar)
      ImGui::MenuItem(prop.Name, "Ctrl+W?", &prop.Opened);
  }
}
