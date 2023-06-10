#include "Camera/3dView.h"

#include "Camera/3dAction.h"
#include "Camera/ControllerView.h"
#include "Context.h"
#include "Widgets/Utils.h"

using namespace Ui;

void Camera3dView::DrawOptions(Context& ctx, Cellnta::Camera3d& cam) {
  CELLNTA_PROFILE;

  float mouseSpeed = cam.GetMouseSpeed();
  if (ImGui::DragFloat("Mouse sensitivity", &mouseSpeed, 0.001f, 0.0f, 100.0f))
    ctx.PushAction(Action::Make(Action::Camera3d::SetMouseSpeed(mouseSpeed)));

  float moveSpeed = cam.GetMoveSpeed();
  if (ImGui::DragFloat("Move speed", &moveSpeed, 0.001f, 0.0f, 100.0f))
    ctx.PushAction(Action::Make(Action::Camera3d::SetMoveSpeed(moveSpeed)));
}

void Camera3dView::Draw(Context& ctx, Cellnta::Camera3d& cam) {
  CELLNTA_PROFILE;

  ImGui::PushID(3);  // 3 is dimension

  Eigen::VectorXf pos = cam.GetPosition();
  if (ImGui::DragFloat3("Position##CameraPos", pos.data(), 0.01f))
    ctx.PushAction(Action::Make(Action::Camera3d::SetPosition(pos)));

  constexpr float ItemWidth = 75.0f;
  constexpr float DragRotateSpeed = 0.002f;

  float yaw = cam.GetYaw();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Yaw##Camera", &yaw, DragRotateSpeed))
    ctx.PushAction(Action::Make(Action::Camera3d::SetYaw(yaw)));

  ImGui::SameLine();

  float pitch = cam.GetPitch();
  ImGui::SetNextItemWidth(ItemWidth);
  if (ImGui::DragFloat("Pitch##Camera", &pitch, DragRotateSpeed))
    ctx.PushAction(Action::Make(Action::Camera3d::SetPitch(pitch)));

  ImGui::Spacing();

  bool pressed = false;
  auto pressedType = CameraControllerView::Draw(cam, &pressed);
  switch (pressedType) {
    case CameraControllerView::PressedType::USE_PERSPECTIVE:
      ctx.PushAction(Action::Make(Action::Camera3d::SetUsePerspective(pressed)));
      break;
    case CameraControllerView::PressedType::NONE: break;
  }

  ImGui::PopID();
}
