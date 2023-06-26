#include "Scene/Window.h"

#include <SDL.h>

#include <Cellnta/Renderer/Drawer/GL/GL.h>
#include <Cellnta/Renderer/Camera3d.h>

#include "Context.h"
#include "Log.h"

using namespace Ui;

SceneWindow::SceneWindow() : Window(DefaultProperties) {
  glGenFramebuffers(1, &m_framebuffer);
  glGenRenderbuffers(1, &m_renBuffer);
  glGenTextures(1, &m_texture);
}

SceneWindow::~SceneWindow() {
  glDeleteFramebuffers(1, &m_framebuffer);
  glDeleteRenderbuffers(1, &m_renBuffer);
  glDeleteTextures(1, &m_texture);
}

void SceneWindow::Draw() {
  CELLNTA_PROFILE;

  constexpr ImGuiWindowFlags winFlags =
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar |
      ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoMove;

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

  DESKTOP_LOG_TRACE("Scene drawed");
}

void SceneWindow::SetFocused(bool focused) {
  if (focused == m_focused)
    return;

  if (focused)
    OnEnterFocus();
  else
    OnLeaveFocus();
}

void SceneWindow::OnEnterFocus() {
  CELLNTA_PROFILE;

  SDL_GetMouseState(&m_oldMousePosX, &m_oldMousePosY);

  SDL_SetRelativeMouseMode(SDL_TRUE);
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
  m_focused = true;
}

void SceneWindow::OnLeaveFocus() {
  CELLNTA_PROFILE;

  SDL_SetRelativeMouseMode(SDL_FALSE);
  ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
  m_focused = false;

  SDL_Window* focusWin = SDL_GetMouseFocus();
  if (focusWin == nullptr) {
    DESKTOP_LOG_ERROR("Unable to set mouse position that was before the"
                      "SceneWindow was focused");
    return;
  }

  SDL_WarpMouseInWindow(focusWin, m_oldMousePosX, m_oldMousePosY);
}

void SceneWindow::HandleInput() {
  CELLNTA_PROFILE;

  Context* ctx = GetContext();
  Cellnta::Renderer& ren = ctx->GetRenderer();
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
  if (size != m_framebufferSize) {
    Cellnta::Camera3d* cam = ren.GetCamera3d();

    if (cam != nullptr)
      cam->Resize(Eigen::Vector2f(size.x, size.y));
    ResizeFramebuffer(size.x, size.y);
  }

  if (ImGui::IsKeyPressed(ImGuiKey_Q, true))
    ren.Rotate();
}

void SceneWindow::HandleCameraInput() {
  CELLNTA_PROFILE;

  using Direction = Cellnta::MoveDirection;

  auto IsPressed = []<typename... Args>(Args... keys) -> bool {
    for (auto key : {keys...})
      if (ImGui::IsKeyDown(key))
        return true;
    return false;
  };

  Context* ctx = GetContext();
  Cellnta::Camera3d* cam = GetContext()->GetRenderer().GetCamera3d();
  ImGuiIO& io = ImGui::GetIO();
  float& delta = io.DeltaTime;

  if (cam == nullptr)
    return;

  float deltaX = ctx->GetMouseDeltaX();
  float deltaY = ctx->GetMouseDeltaY();
  if (deltaX != 0 || deltaY != 0) {
    cam->Rotate(deltaX, deltaY, delta);
  }

  if (io.MouseWheel != 0)
    cam->Move(Direction::FORWARD, io.MouseWheel / 4.0f);

  if (IsPressed(ImGuiKey_W, ImGuiKey_UpArrow))
    cam->Move(Direction::FORWARD, delta);
  if (IsPressed(ImGuiKey_A, ImGuiKey_LeftArrow))
    cam->Move(Direction::LEFT, delta);
  if (IsPressed(ImGuiKey_S, ImGuiKey_DownArrow))
    cam->Move(Direction::BACKWARD, delta);
  if (IsPressed(ImGuiKey_D, ImGuiKey_RightArrow))
    cam->Move(Direction::RIGHT, delta);

  if (IsPressed(ImGuiKey_Space))
    cam->Move(Direction::WORLD_UP, delta);
  if (IsPressed(ImGuiKey_LeftShift))
    cam->Move(Direction::WORLD_DOWN, delta);
}

void SceneWindow::ResizeFramebuffer(int width, int height) {
  CELLNTA_PROFILE;

  DESKTOP_LOG_TRACE("Resizing framebuffer to: ({}; {})", width, height);

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

  glBindRenderbuffer(GL_RENDERBUFFER, m_renBuffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renBuffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    DESKTOP_LOG_ERROR("Could not create scene framebuffer");

  m_framebufferSize = ImVec2(width, height);
}

