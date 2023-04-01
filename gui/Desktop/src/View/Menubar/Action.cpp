#include "View/Menubar/Action.h"

#include <SDL2/SDL.h>

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
  assert(0 && "Not implemented");
}

void ZoomOut::Execute() {
  assert(0 && "Not implemented");
}

void ResetZoom::Execute() {
  assert(0 && "Not implemented");
}
