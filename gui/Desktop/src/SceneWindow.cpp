#include "SceneWindow.h"

#include <SDL.h>

#include <Cellnta/Renderer/GlBackend.h>
#include <Cellnta/Log.h>

#include "Widgets.h"
#include "Context.h"

using namespace Ui;

SceneWindow::~SceneWindow()
{
  if (m_framebuffer != 0)
    glDeleteFramebuffers(1, &m_framebuffer);
  if (m_texture != 0)
    glDeleteTextures(1, &m_texture);
  if (m_depthTexture != 0)
    glDeleteTextures(1, &m_depthTexture);
}

void SceneWindow::Draw()
{
  ProfileScope;

  constexpr ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoBringToFrontOnFocus
    | ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoCollapse
    | ImGuiWindowFlags_NoMove;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

  Cellnta::Canvas& canvas = GetContext()->GetCanvas();
  ImGuiIO io = ImGui::GetIO();

  if (ImGui::Begin(p_prop.Name, nullptr, winFlags))
  {
    if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
      if(io.KeyCtrl && m_focused)
        OnLeaveFocus();
      else if(io.KeyCtrl && ImGui::IsWindowFocused())
        OnEnterFocus();
    }

    if(m_focused)
      HandleInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    if (size.x != m_framebufferSize.x || size.y != m_framebufferSize.y)
    {
      canvas.OnResize(size.x, size.y);
      ResizeFramebuffer(size.x, size.y);
    }

    if (canvas.WantDraw())
      DrawGlScene();

    ImGui::Image((void*)(intptr_t) m_texture, size,
      ImVec2(0, 1), ImVec2(1, 0));
  }
  ImGui::PopStyleVar(3);
  ImGui::End();
}

void SceneWindow::DrawGlScene()
{
  Cellnta::Canvas& canvas = GetContext()->GetCanvas();

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
#if LF_TARGET_GLES3
  glClearDepthf(1.0f);
#else
  glClearDepth(1.0f);
#endif

  glViewport(0, 0, m_framebufferSize.x, m_framebufferSize.y);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  canvas.RenderWorld();
  canvas.RenderGrid();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void SceneWindow::OnEnterFocus()
{
  ProfileScope;

  m_focused = true;
  SDL_SetRelativeMouseMode(SDL_TRUE);
  //io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void SceneWindow::OnLeaveFocus()
{
  ProfileScope;

  m_focused = false;
  SDL_SetRelativeMouseMode(SDL_FALSE);
  //io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void SceneWindow::HandleInput()
{
  ProfileScope;

  Cellnta::Canvas& canvas = GetContext()->GetCanvas();
  ImGuiIO& io = ImGui::GetIO();
  float& delta = io.DeltaTime;

  if(io.MouseDelta.x != 0 || io.MouseDelta.y != 0)
    canvas.OnMouseMotion(io.MouseDelta.x, io.MouseDelta.y, delta);
  if(io.MouseWheel != 0)
    canvas.OnMove(Cellnta::MoveDirection::FORWARD, io.MouseWheel / 4.0f);

  if(ImGui::IsKeyDown(ImGuiKey_W) || ImGui::IsKeyDown(ImGuiKey_UpArrow))
    canvas.OnMove(Cellnta::MoveDirection::FORWARD, delta);
  if(ImGui::IsKeyDown(ImGuiKey_A) || ImGui::IsKeyDown(ImGuiKey_LeftArrow))
    canvas.OnMove(Cellnta::MoveDirection::LEFT, delta);
  if(ImGui::IsKeyDown(ImGuiKey_S) || ImGui::IsKeyDown(ImGuiKey_DownArrow))
    canvas.OnMove(Cellnta::MoveDirection::BACKWARD, delta);
  if(ImGui::IsKeyDown(ImGuiKey_D) || ImGui::IsKeyDown(ImGuiKey_RightArrow))
    canvas.OnMove(Cellnta::MoveDirection::RIGHT, delta);

  if(ImGui::IsKeyDown(ImGuiKey_Space))
    canvas.OnMove(Cellnta::MoveDirection::WORLD_UP, delta);
  if(ImGui::IsKeyDown(ImGuiKey_LeftShift))
    canvas.OnMove(Cellnta::MoveDirection::WORLD_DOWN, delta);
}

void SceneWindow::ResizeFramebuffer(int width, int height)
{
  ProfileScope;

  if (m_framebuffer == 0)
    glGenFramebuffers(1, &m_framebuffer);
  if (m_texture == 0)
    glGenTextures(1, &m_texture);
  if (m_depthTexture == 0)
    glGenTextures(1, &m_depthTexture);

  LOG_INFO("Resizing framebuffer to: ({}; {})", width, height);

  glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, m_texture, 0);

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
