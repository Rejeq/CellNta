#pragma once

#include <cstdint>

#include "WindowBase.h"

namespace Ui {

class SceneWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Scene",
      .VisibleInMenubar = false,
  };

 public:
  SceneWindow();
  ~SceneWindow();

  void Draw() override;
  void DrawGlScene();

  ImVec2 GetFramebufferSize() const { return m_framebufferSize; }
  uint32_t GetFramebuffer() const { return m_framebuffer; }
  uint32_t GetTexture() const { return m_texture; }

  void SetFocused(bool focused);
  bool GetFocused() const { return m_focused; }

 private:
  void OnEnterFocus();
  void OnLeaveFocus();
  void HandleInput();
  void HandleCameraInput();
  void ResizeFramebuffer(int width, int height);

  ImVec2 m_framebufferSize = ImVec2(0, 0);
  int m_oldMousePosX = 0;
  int m_oldMousePosY = 0;
  uint32_t m_framebuffer = 0;
  uint32_t m_renBuffer = 0;
  uint32_t m_texture = 0;
  bool m_focused = false;
};


}
