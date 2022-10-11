#include "Context.h"

#include <vector>
#include <memory>

#include "Widgets.h"

namespace Ui
{
  static std::unique_ptr<Context> s_ctx;

  Context::Context(std::unique_ptr<Canvas>&& canvas)
  {
    m_canvas = std::move(canvas);
    ImGuiContext* imguiCtx = ImGui::GetCurrentContext();
    ImGuiSettingsHandler contextHandler;
    contextHandler.TypeName = "CellNta";
    contextHandler.TypeHash = ImHashStr("CellNta");
    contextHandler.UserData = (void*)this;
    contextHandler.ReadOpenFn = SettingsHandler_ReadOpen;
    contextHandler.ReadLineFn = SettingsHandler_ReadLine;
    contextHandler.WriteAllFn = SettingsHandler_WriteAll;
    imguiCtx->SettingsHandlers.push_back(contextHandler);
  }

  void Context::AddWindow(std::unique_ptr<Window>&& window)
  {
    window->SetContext(this);
    m_windowsData.push_back(std::move(window));
  }

  void Context::Draw()
  {
    ImGui::PushID("Context name"); //TODO

    ProfileScope;

    if (FirstStartup)
    {
      //ResetLayout();
      FirstStartup = false;
    }

    ImGui::BeginMainMenuBar();

    ImVec4 activeColor = ImGui::GetStyle().Colors[ImGuiCol_Button];
    activeColor.y *= 1.5f;
    activeColor.z *= 0.6f;

    ImGui::PushStyleColor(ImGuiCol_ButtonActive, activeColor);
    for (auto& window : m_windowsData)
      Widget::ToggleButton(window->GetName(), window->GetOpened());
    ImGui::PopStyleColor();

    const float maxWidth = ImGui::GetContentRegionMax().x;
    float framerateWidth = 0;
    framerateWidth = DrawFramerate(maxWidth);

    //TODO:
    constexpr const char* ResetLayoutStr = "Reset layout";
    float resetLayoutWidth = ImGui::CalcTextSize(ResetLayoutStr).x;

    const float spacing = ImGui::GetStyle().ItemSpacing.x;
    ImGui::SetCursorPosX(maxWidth - framerateWidth - resetLayoutWidth - spacing - 10);
    if (ImGui::Button(ResetLayoutStr))
      ResetLayout();

    ImGui::EndMainMenuBar();

    DrawScene();
    for (auto& window : m_windowsData)
    {
      if (window->GetOpened())
        window->Draw();
    }
    ImGui::PopID();
  }

  void* Context::SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler* handler, const char* name)
  {
    Context* ctx = (Context*) handler->UserData;
    ctx->FirstStartup = false;
    ctx->m_currentWindow.clear();
    return (void*)ctx;
  }

  void Context::SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line)
  {
    Context* ctx = (Context*)entry;

    if (line[0] == '(')
    {
      size_t size = std::strlen(line);
      if (line[size - 1] == ')')
      {
        ctx->m_currentWindow = std::string(line + 1, size - 2);
      }
    }
    else if(!ctx->m_currentWindow.empty())
    {
      for (auto& window: ctx->m_windowsData)
      {
        if (ctx->m_currentWindow == window->GetName())
        {
          window->ReadProperties(line);
        }
      }
    }
  }

  void Context::SettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
  {
    if (handler->UserData == nullptr)
      return;

    Context* ctx = (Context*)handler->UserData;
    buf->reserve(buf->size() + 6 * 6);
    buf->appendf("[%s][%s]\n", "CellNta", "Context");
    ctx->WriteWindowProperties(handler, buf);
    buf->append("\n");
  }

  char* Context::GetTmpBuffer()
  {
    ImGuiContext& g = *GImGui;
    return g.TempBuffer.Data;
  }

  char* Context::GetTmpBuffer(size_t& size)
  {
    ImGuiContext& g = *GImGui;
    //Need return specified size
    //if (size != 0)
    //  if (size > g.TempBuffer.Capacity)
    //    g.TempBuffer.resize(size);
    size = g.TempBuffer.Size;
    return g.TempBuffer.Data;
  }

  float Context::DrawFramerate(const float& offset)
  {
    constexpr const char* FramrateStr = "FPS: %i(%.3fms)";
    const float framerate = ImGui::GetIO().Framerate;
    size_t size = 0;
    char* text = GetTmpBuffer(size);

    snprintf(text, size, FramrateStr, (int)framerate, 1000.0f / framerate);
    const float textWidth = ImGui::CalcTextSize(text).x;

    if (offset > 0)
      ImGui::SetCursorPosX(offset - textWidth);
    ImGui::TextUnformatted(text);
    return textWidth;
  }

  void Context::DrawScene()
  {
    const ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoBringToFrontOnFocus
      | ImGuiWindowFlags_NoResize    | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse
      | ImGuiWindowFlags_NoTitleBar  | ImGuiWindowFlags_NoCollapse
      | ImGuiWindowFlags_NoMove;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    if (ImGui::Begin("Scene", nullptr, winFlags))
    {
      SceneWindowFocused = ImGui::IsWindowFocused();

      static ImVec2 oldSize = ImVec2(0, 0);
      ImVec2 size = ImGui::GetContentRegionAvail();

      if (size.x != oldSize.x || size.y != oldSize.y)
        m_canvas->OnResize(size.x, size.y);

      ImGui::Image((void*)SceneTextureId, size,
        ImVec2(0, 1), ImVec2(1, 0));

      oldSize = size;
    }
    ImGui::PopStyleVar(3);
    ImGui::End();
  }

  void Context::ResetLayout()
  {
    ImGuiID dockId = MainDockspaceId;

    ImGui::DockBuilderRemoveNode(dockId);
    ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderGetNode(dockId)->LocalFlags &= ~ImGuiDockNodeFlags_CentralNode;

    constexpr float size = 0.25f;
    ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Left, size, NULL, &dockId);
    ImGuiID dockIdRem = ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Right, 1.0f - size, NULL, &dockId);

    ImGui::DockBuilderDockWindow("Scene", dockIdRem);
    ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockIdRem); //TODO: Delete this

    ImGui::DockBuilderDockWindow("Renderer", dockIdLeft);
    ImGui::DockBuilderDockWindow("Algorithm", dockIdLeft);

    ImGui::DockBuilderFinish(dockId);
  }

  void Context::WriteWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
  {
    for (auto& window : m_windowsData)
    {
      buf->appendf("(%s)\n", window->GetName());
      window->WriteProperties(buf);
      buf->appendf("\n");
    }
  }
}
