#include "View/Camera/Window.h"

// FIXME: Need use external fmt
// Not using std::format because clang still dosen't support it
#include <spdlog/fmt/fmt.h>

#include "Context.h"
#include "View/Camera/Action.h"
#include "Widgets/Utils.h"

using namespace Ui;

void CameraWindow::Draw() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  Cellnta::Camera3d* camera3d = ren.GetCamera3d();
  Cellnta::CameraNdList* cameraNd = ren.GetCameraNd();

  if (ImGui::Begin(p_prop.Name, &p_prop.Opened)) {
    if (camera3d != nullptr)
      DrawCameraOptions(*camera3d);

    if (cameraNd != nullptr)
      DrawCameraNdList(*cameraNd);

    if (camera3d != nullptr) {
      if (ImGui::TreeNodeEx("3d camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        DrawCamera3d(*camera3d);
        ImGui::TreePop();
      }
    }
  }

  ImGui::End();
}

void CameraWindow::DrawCameraOptions(Cellnta::Camera3d& camera) {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();

  float mouseSpeed = camera.GetMouseSpeed();
  if (ImGui::DragFloat("Mouse sensitivity", &mouseSpeed, 0.001f, 0.0f, 100.0f))
    ctx->PushAction(Action::Make(Action::Camera3d::SetMouseSpeed(mouseSpeed)));

  float moveSpeed = camera.GetMoveSpeed();
  if (ImGui::DragFloat("Move speed", &moveSpeed, 0.001f, 0.0f, 100.0f))
    ctx->PushAction(Action::Make(Action::Camera3d::SetMoveSpeed(moveSpeed)));
}

void CameraWindow::DrawCamera3d(Cellnta::Camera3d& cam3d) {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();

  ImGui::PushID(3);  // 3 is dimension

  Eigen::VectorXf pos = cam3d.GetPosition();
  if (ImGui::DragFloat3("Position##CameraPos", pos.data(), 0.01f))
    ctx->PushAction(Action::Make(Action::Camera3d::SetPosition(pos)));

  constexpr float ItemWidth = 75.0f;
  constexpr float DragRotateSpeed = 0.002f;

  float yaw = cam3d.GetYaw();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Yaw##Camera", &yaw, DragRotateSpeed))
    ctx->PushAction(Action::Make(Action::Camera3d::SetYaw(yaw)));

  ImGui::SameLine();

  float pitch = cam3d.GetPitch();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Pitch##Camera", &pitch, DragRotateSpeed))
    ctx->PushAction(Action::Make(Action::Camera3d::SetPitch(pitch)));

  ImGui::Spacing();

  DrawCameraController(cam3d, [](bool perspective) -> Action::BasePtr {
    return Action::Make(Action::Camera3d::SetUsePerspective(perspective));
  });

  ImGui::PopID();
}

void CameraWindow::DrawCameraNdList(Cellnta::CameraNdList& list) {
  CELLNTA_PROFILE;

  for (int i = 0; i < list.size(); ++i) {
    Cellnta::CameraNd& cam = list[i];
    std::string camName = fmt::format("{} Camera", cam.GetDimensions());

    if (ImGui::TreeNodeEx(camName.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
      DrawCameraNd(i, cam);
      ImGui::TreePop();
    }
  }
}

void CameraWindow::DrawCameraNd(int idx, Cellnta::CameraNd& camNd) {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();

  ImGui::PushID(camNd.GetDimensions());

  bool skip = camNd.WantSkip();
  if (ImGui::Checkbox("Skip", &skip))
    ctx->PushAction(Action::Make(Action::CameraNd::SetNeedSkip(idx, skip)));

  if (skip)
    ImGui::BeginDisabled();

  constexpr float DragSpeed = 0.01f;

  Eigen::VectorXf pos = camNd.GetPosition();
  if (Widget::DragN("Position##CameraPos", pos.data(), pos.size(), DragSpeed))
    ctx->PushAction(Action::Make(Action::CameraNd::SetPosition(idx, pos)));

  Eigen::VectorXf front = camNd.GetFront();
  if (Widget::DragN("Target##CameraPos", front.data(), front.size(), DragSpeed))
    ctx->PushAction(Action::Make(Action::CameraNd::SetFront(idx, front)));

  ImGui::Spacing();

  DrawCameraController(camNd, [&](bool perspective) -> Action::BasePtr {
    return Action::Make(Action::CameraNd::SetUsePerspective(idx, perspective));
  });

  if (skip)
    ImGui::EndDisabled();

  ImGui::PopID();
}

template <typename PerspectiveLambda>
void CameraWindow::DrawCameraController(
    Cellnta::CameraController& controller,
    PerspectiveLambda&& usePrespectiveAction) {
  constexpr const char* PerspectiveStr = "Current perspective";
  constexpr const char* OrthoStr = "Current orthographic";

  Context* ctx = GetContext();
  bool perspective = controller.GetUsePerspective();
  const char* perspectiveName = (perspective) ? PerspectiveStr : OrthoStr;

  if (ImGui::Checkbox(perspectiveName, &perspective))
    ctx->PushAction(usePrespectiveAction(perspective));
}
