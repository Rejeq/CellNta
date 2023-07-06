#include "Menubar/AppearanceAction.h"

#include <SDL.h>

#include "Log.h"

using namespace Ui;
using namespace Ui::Action::Appearance;

void ToggleFullscreen::Execute() {
  constexpr SDL_WindowFlags fullscreenFlag = SDL_WINDOW_FULLSCREEN_DESKTOP;

  SDL_Window* win = SDL_GL_GetCurrentWindow();

  if (Action::CheckLogErr(
          !win, DESKTOP_ACTION_DERR_MSG "Appearance::ToggleFullscreen: {}",
          SDL_GetError()))
    return;

  bool isFullscreen = SDL_GetWindowFlags(win) & fullscreenFlag;
  SDL_SetWindowFullscreen(win, (isFullscreen) ? 0 : fullscreenFlag);
}

void ZoomIn::Execute() {
  DESKTOP_LOG_ERROR("ZoomIn action not implemented");
}

void ZoomOut::Execute() {
  DESKTOP_LOG_ERROR("ZoomOut action not implemented");
}

void ResetZoom::Execute() {
  DESKTOP_LOG_ERROR("ResetZoom action not implemented");
}

