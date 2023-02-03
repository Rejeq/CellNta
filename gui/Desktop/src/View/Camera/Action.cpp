#include "View/Camera/Action.h"

#include <Cellnta/Log.h>
#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"

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
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetMouseSpeed action");
    return;
  }

  cam->SetMouseSpeed(m_speed);
}

void SetMoveSpeed::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetMoveSpeed action");
    return;
  }

  cam->SetMoveSpeed(m_speed);
}

void SetPosition::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetPosition action");
    return;
  }

  cam->SetPosition(m_pos);
}

void SetYaw::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetYaw action");
    return;
  }

  cam->SetYaw(m_yaw);
}

void SetPitch::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetPitch action");
    return;
  }

  cam->SetPitch(m_pitch);
}

void SetUsePerspective::Execute() {
  Cellnta::Camera3d* cam = GetCamera3d(p_ctx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Camera3d::SetUsePerspective action");
    return;
  }

  cam->SetUsePerspective(m_perspective);
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
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute CameraNd::ToggleNeedSkip action");
    return;
  }

  bool skip = cam->WantSkip();
  skip = !skip;
  cam->NeedSkip(skip);
}

void SetNeedSkip::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute CameraNd::SetNeedSkip action");
    return;
  }

  cam->NeedSkip(m_skip);
}

void SetPosition::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute CameraNd::SetPosition action");
    return;
  }

  cam->SetPosition(m_pos);
}

void SetFront::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute CameraNd::SetPosition action");
    return;
  }

  cam->SetFront(m_front);
}

void SetUsePerspective::Execute() {
  Cellnta::CameraNd* cam = GetCameraNd(p_ctx, m_idx);
  if (cam == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute CameraNd::SetUsePerspective action");
    return;
  }

  cam->SetUsePerspective(m_perspective);
}

}  // namespace Ui::Action::CameraNd
