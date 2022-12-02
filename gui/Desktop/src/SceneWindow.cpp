#include "SceneWindow.h"

#include <SDL.h>

#include <Cellnta/Log.h>
#include <Cellnta/Renderer/GlBackend.h>

#include "Context.h"
#include "Widgets.h"

using namespace Ui;

SceneWindow::~SceneWindow() {
  if (m_framebuffer != 0)
    glDeleteFramebuffers(1, &m_framebuffer);
  if (m_texture != 0)
    glDeleteTextures(1, &m_texture);
  if (m_depthTexture != 0)
    glDeleteTextures(1, &m_depthTexture);
}

void SceneWindow::Draw() {
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags winFlags =
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  Cellnta::Renderer& ren = GetContext()->GetRenderer();

  if (ImGui::Begin(p_prop.Name, nullptr, winFlags)) {
    HandleInput();

    if (ren.WantDraw())
      DrawGlScene();

    ImGui::Image((void*)(intptr_t)m_texture, m_framebufferSize, ImVec2(0, 1),
                 ImVec2(1, 0));
  }
  ImGui::PopStyleVar(3);
  ImGui::End();
}

void SceneWindow::DrawGlScene() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
#if CELLNTA_RENDERER_GLES3
  glClearDepthf(1.0f);
#else
  glClearDepth(1.0f);
#endif

  glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ren.Draw();
  ren.DrawGrid();

  CELLNTA_LOG_TRACE("Scene drawed");
}

void SceneWindow::OnEnterFocus() {
  CELLNTA_PROFILE;

  m_focused = true;
  SDL_SetRelativeMouseMode(SDL_TRUE);
  // io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void SceneWindow::OnLeaveFocus() {
  CELLNTA_PROFILE;

  m_focused = false;
  SDL_SetRelativeMouseMode(SDL_FALSE);
  // io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void SceneWindow::HandleInput() {
  CELLNTA_PROFILE;

  Cellnta::Renderer& ren = GetContext()->GetRenderer();
  ImGuiIO io = ImGui::GetIO();

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    if (io.KeyCtrl && m_focused)
      OnLeaveFocus();
    else if (io.KeyCtrl && ImGui::IsWindowFocused())
      OnEnterFocus();
  }

  if (m_focused)
    HandleCameraInput();

  ImVec2 size = ImGui::GetContentRegionAvail();

  if (size.x != m_framebufferSize.x || size.y != m_framebufferSize.y) {
    ren.GetCamera3d().Resize(Eigen::Vector2f(size.x, size.y));
    ResizeFramebuffer(size.x, size.y);
  }

  if (ImGui::IsKeyPressed(ImGuiKey_Q, true))
    ren.Rotate(0, 0, 0);
}

void SceneWindow::HandleCameraInput() {
  CELLNTA_PROFILE;

  auto IsPressed = []<typename... Args>(Args... keys) -> bool {
    for (auto key : {keys...})
      if (ImGui::IsKeyDown(key))
        return true;
    return false;
  };

  Cellnta::Camera3d& cam = GetContext()->GetRenderer().GetCamera3d();
  ImGuiIO& io = ImGui::GetIO();
  float& delta = io.DeltaTime;

  if (io.MouseDelta.x != 0 || io.MouseDelta.y != 0)
    cam.Rotate(io.MouseDelta.x, io.MouseDelta.y, delta);
  if (io.MouseWheel != 0)
    cam.Move(Cellnta::MoveDirection::FORWARD, io.MouseWheel / 4.0f);

  if (IsPressed(ImGuiKey_W, ImGuiKey_UpArrow))
    cam.Move(Cellnta::MoveDirection::FORWARD, delta);
  if (IsPressed(ImGuiKey_A, ImGuiKey_LeftArrow))
    cam.Move(Cellnta::MoveDirection::LEFT, delta);
  if (IsPressed(ImGuiKey_S, ImGuiKey_DownArrow))
    cam.Move(Cellnta::MoveDirection::BACKWARD, delta);
  if (IsPressed(ImGuiKey_D, ImGuiKey_RightArrow))
    cam.Move(Cellnta::MoveDirection::RIGHT, delta);

  if (IsPressed(ImGuiKey_Space))
    cam.Move(Cellnta::MoveDirection::WORLD_UP, delta);
  if (IsPressed(ImGuiKey_LeftShift))
    cam.Move(Cellnta::MoveDirection::WORLD_DOWN, delta);
}

void SceneWindow::ResizeFramebuffer(int width, int height) {
  CELLNTA_PROFILE;

  if (m_framebuffer == 0)
    glGenFramebuffers(1, &m_framebuffer);
  if (m_texture == 0)
    glGenTextures(1, &m_texture);
  if (m_depthTexture == 0)
    glGenTextures(1, &m_depthTexture);

  CELLNTA_LOG_INFO("Resizing framebuffer to: ({}; {})", width, height);

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         m_texture, 0);

  glBindTexture(GL_TEXTURE_2D, m_depthTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
               GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, m_depthTexture, 0);

  m_framebufferSize = ImVec2(width, height);
}
