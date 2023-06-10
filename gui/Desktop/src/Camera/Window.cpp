#include "Camera/Window.h"

#include "Camera/3dView.h"
#include "Camera/NdView.h"
#include "Context.h"

using namespace Ui;

void CameraWindow::Draw() {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();
  if (ctx == nullptr)
    return;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    Cellnta::Camera3d* camera3d = ren.GetCamera3d();
    if (camera3d != nullptr) {
      Camera3dView::DrawOptions(*ctx, *camera3d);
    }

    Cellnta::CameraNdList* cameraNd = ren.GetCameraNd();
    if (cameraNd != nullptr) {
      CameraNdView::Draw(*ctx, *cameraNd);
    }

    if (camera3d != nullptr) {
      if (ImGui::TreeNodeEx("3d camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        Camera3dView::Draw(*ctx, *camera3d);
        ImGui::TreePop();
      }
    }
  }

  ImGui::End();
}
