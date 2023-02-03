#include "View/Scene/Action.h"

#include <Cellnta/Log.h>

#include "Context.h"
#include "View/Scene/Window.h"  // FIXME: Delete this

using namespace Ui;
using namespace Ui::Action::Scene;

static SceneWindow* GetSceneWindow(Context* ctx) {
  if (ctx == nullptr)
    return nullptr;

  Window* win = ctx->GetWindowByName("Scene");
  if (win == nullptr) {
    CELLNTA_LOG_WARN("Scene window not exist");
    return nullptr;
  }
  return (SceneWindow*)win;
}

void ToggleFocus::Execute() {
  SceneWindow* win = GetSceneWindow(p_ctx);
  if (win == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Scene::ToggleFocus action");
    return;
  }

  bool focus = win->GetFocused();
  focus = !focus;
  win->SetFocused(focus);
}

void LeaveFocus::Execute() {
  SceneWindow* win = GetSceneWindow(p_ctx);
  if (win == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Scene::ToggleFocus action");
    return;
  }

  win->SetFocused(false);
}

void EnterFocus::Execute() {
  SceneWindow* win = GetSceneWindow(p_ctx);
  if (win == nullptr) {
    CELLNTA_LOG_ERROR("Unable to execute Scene::ToggleFocus action");
    return;
  }

  win->SetFocused(true);
}
