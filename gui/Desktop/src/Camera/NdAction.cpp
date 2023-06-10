#include "Camera/NdAction.h"

#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"
#include "Log.h"

using namespace Ui;

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
