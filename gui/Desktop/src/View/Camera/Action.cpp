#include "View/Camera/Action.h"

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"
#include "Log.h"

using namespace Ui;

namespace Ui::Action::Camera3d {

static Cellnta::Camera3d* GetCamera3d(Context* ctx) {
  if (ctx == nullptr)
    return nullptr;

  Cellnta::Renderer& ren = ctx->GetRenderer();
  return ren.GetCamera3d();
}

void SetMouseSpeed::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetMouseSpeed"))
    return;

  m_prevSpeed = cam->GetMouseSpeed();
  cam->SetMouseSpeed(m_speed);
}

void SetMouseSpeed::Undo() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetMouseSpeed"))
    return;

  cam->SetMouseSpeed(m_prevSpeed);
}

void SetMoveSpeed::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetMoveSpeed"))
    return;

  m_prevSpeed = cam->GetMoveSpeed();
  cam->SetMoveSpeed(m_speed);
}

void SetMoveSpeed::Undo() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetMoveSpeed"))
    return;

  cam->SetMoveSpeed(m_prevSpeed);
}

void SetPosition::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetPosition"))
    return;

  cam->SetPosition(m_pos);
}

void SetYaw::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetYaw"))
    return;

  cam->SetYaw(m_yaw);
}

void SetPitch::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetPitch"))
    return;

  cam->SetPitch(m_pitch);
}

void SetUsePerspective::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetUsePerspective"))
    return;

  m_prevPerspective = cam->GetUsePerspective();
  cam->SetUsePerspective(m_perspective);
}

void SetUsePerspective::Undo() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (Action::CheckLogDErr(!cam, "Camera3d::SetUsePerspective"))
    return;

  cam->SetUsePerspective(m_prevPerspective);
}

}  // namespace Ui::Action::Camera3d

namespace Ui::Action::CameraNd {

static Cellnta::CameraNd* GetCameraNd(Context* ctx, int idx) {
  if (ctx == nullptr)
    return nullptr;

  Cellnta::Renderer& ren = ctx->GetRenderer();
  return &ren.GetCameraNd()->at(idx);
}

void ToggleNeedSkip::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::ToggleNeedSkip"))
    return;

  bool skip = cam->WantSkip();
  skip = !skip;
  cam->NeedSkip(skip);
}

void SetNeedSkip::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetNeedSkip"))
    return;

  m_prevSkip = cam->WantSkip();
  cam->NeedSkip(m_skip);
}

void SetNeedSkip::Undo() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetNeedSkip"))
    return;

  cam->NeedSkip(m_prevSkip);
}

void SetPosition::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetPositon"))
    return;

  cam->SetPosition(m_pos);
}

void SetFront::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetFront"))
    return;

  cam->SetFront(m_front);
}

void SetUsePerspective::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetUsePerspective"))
    return;

  m_prevPerspective = cam->GetUsePerspective();
  cam->SetUsePerspective(m_perspective);
}

void SetUsePerspective::Undo() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (Action::CheckLogDErr(!cam, "CameraNd::SetUsePerspective"))
    return;

  cam->SetUsePerspective(m_prevPerspective);
}

}  // namespace Ui::Action::CameraNd
