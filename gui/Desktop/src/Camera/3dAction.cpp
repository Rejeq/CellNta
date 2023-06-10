#include "Camera/3dAction.h"

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"
#include "Log.h"

using namespace Ui;
using namespace Ui::Action::Camera3d;

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
