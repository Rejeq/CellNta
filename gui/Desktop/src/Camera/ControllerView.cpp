#include "Camera/ControllerView.h"

#include <imgui.h>

#include <Cellnta/Config.h>

using namespace Ui;
using CamContView = CameraControllerView;

CamContView::PressedType CamContView::Draw(
    const Cellnta::CameraController& cont, bool* value) {
  CELLNTA_PROFILE;

  constexpr const char* PerspectiveStr = "Current perspective";
  constexpr const char* OrthoStr = "Current orthographic";

  bool perspective = cont.GetUsePerspective();
  const char* perspectiveName = (perspective) ? PerspectiveStr : OrthoStr;

  if (ImGui::Checkbox(perspectiveName, &perspective)) {
    *value = perspective;
    return PressedType::USE_PERSPECTIVE;
  }

  return PressedType::NONE;
}
