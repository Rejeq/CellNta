#include "Context.h"

#include <vector>
#include <memory>

#include "Widgets.h"

using namespace Ui;

static std::unique_ptr<Context> s_ctx;

Context::Context()
{
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
  ProfileScope;

  ImGui::PushID("Context name"); //TODO

  if (m_firstStartup)
  {
    if(m_OnFirstStartup)
      m_OnFirstStartup(*this);
    m_firstStartup = false;
  }

  for (auto& window : m_windowsData)
    if (window->GetProperties().Opened)
      window->Draw();

  ImGui::PopID();
}

Window* Context::GetWindowByName(const std::string& name)
{
  ProfileScope;

  for(const auto& win: m_windowsData)
  {
    if(name == win->GetProperties().Name)
      return win.get();
  }

  return nullptr;
}

void* Context::SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler* handler, const char* name)
{
  Context* ctx = (Context*) handler->UserData;
  ctx->m_firstStartup = false;
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
      if (ctx->m_currentWindow == window->GetProperties().Name)
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

void Context::WriteWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf)
{
  for (auto& window : m_windowsData)
  {
    buf->appendf("(%s)\n", window->GetProperties().Name);
    window->WriteProperties(buf);
    buf->appendf("\n");
  }
}
