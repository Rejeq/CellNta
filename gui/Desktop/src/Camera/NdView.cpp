#include "Camera/NdView.h"
//
// FIXME: Need use external fmt
// Not using std::format because clang still dosen't support it
#include <spdlog/fmt/fmt.h>

#include "Camera/ControllerView.h"
#include "Camera/NdAction.h"
#include "Context.h"
#include "Widgets/Utils.h"

using namespace Ui;

void CameraNdView::Draw(Context& ctx, Cellnta::CameraNdList& camList) {
  CELLNTA_PROFILE;

  for (size_t i = 0; i < camList.size(); ++i) {
    Cellnta::CameraNd& cam = camList[i];
    std::string camName = fmt::format("{} Camera", cam.GetDimensions());

    if (ImGui::TreeNodeEx(camName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
      DrawInstance(ctx, cam, i);
      ImGui::TreePop();
    }
  }
}

void CameraNdView::DrawInstance(Context& ctx, Cellnta::CameraNdList& camList,
                                int idx) {
  CELLNTA_PROFILE;

  DrawInstance(ctx, camList[idx], idx);
}

void CameraNdView::DrawInstance(Context& ctx, Cellnta::CameraNd& cam, int idx) {
  CELLNTA_PROFILE;

  ImGui::PushID(cam.GetDimensions());

  bool skip = cam.WantSkip();
  if (ImGui::Checkbox("Skip", &skip))
    ctx.PushAction<Action::CameraNd::SetNeedSkip>(idx, skip);

  if (skip)
    ImGui::BeginDisabled();

  constexpr float DragSpeed = 0.01f;

  Eigen::VectorXf pos = cam.GetPosition();
  if (Widget::DragN("Position##CameraPos", pos.data(), pos.size(), DragSpeed))
    ctx.PushAction<Action::CameraNd::SetPosition>(idx, pos);

  Eigen::VectorXf front = cam.GetFront();
  if (Widget::DragN("Target##CameraPos", front.data(), front.size(), DragSpeed))
    ctx.PushAction<Action::CameraNd::SetFront>(idx, front);

  ImGui::Spacing();

  bool pressed = false;
  auto pressedType = CameraControllerView::Draw(cam, &pressed);
  switch (pressedType) {
    case CameraControllerView::PressedType::USE_PERSPECTIVE:
      ctx.PushAction<Action::CameraNd::SetUsePerspective>(idx, pressed);
      break;
    case CameraControllerView::PressedType::NONE: break;
  }

  if (skip)
    ImGui::EndDisabled();

  ImGui::PopID();
}
