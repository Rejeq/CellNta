#include "Context.h"

#include <memory>
#include <vector>

#include <Cellnta/Algorithms/AlgoList.h>

#include "Widgets.h"

using namespace Ui;

static std::unique_ptr<Context> s_ctx;

Context::Context() {
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

void Context::AddWindow(std::unique_ptr<Window>&& window) {
  window->SetContext(this);
  m_windowsData.push_back(std::move(window));
}

void Context::SetDimension(int dim) {
  CELLNTA_PROFILE;

  m_renderer.SetDimension(dim);
  if (m_algo != nullptr)
    m_algo->SetDimension(dim);
}

void Context::Update() {
  CELLNTA_PROFILE;

  if (m_algo->NeedLoadWorld() || m_renderer.GetData().DesireArea())
    m_algo->LoadWorld(&m_renderer.GetData());

  m_renderer.Update();
}

void Context::Draw() {
  CELLNTA_PROFILE;

  ImGui::PushID("Context name");  // TODO

  SetupDockspace();

  if (m_firstStartup) {
    if (m_OnFirstStartup)
      m_OnFirstStartup(*this);
    m_firstStartup = false;
  }

  // TODO: Delete this
  ImGui::ShowDemoWindow();
  for (auto& window : m_windowsData)
    if (window->GetProperties().Opened)
      window->Draw();

  ImGui::PopID();
}

void Context::SetupDockspace() {
  CELLNTA_PROFILE;

  m_dockspaceId = ImGui::DockSpaceOverViewport(
      nullptr, ImGuiDockNodeFlags_PassthruCentralNode);
}

Window* Context::GetWindowByName(const std::string& name) {
  CELLNTA_PROFILE;

  for (const auto& win : m_windowsData) {
    if (name == win->GetProperties().Name)
      return win.get();
  }

  return nullptr;
}

bool Context::SetAlgo(const Cellnta::AlgoType type) {
  CELLNTA_PROFILE;

  std::unique_ptr<Cellnta::AlgoBase> tmp = Cellnta::CreateAlgoInstance(type);
  if (tmp == nullptr)
    return true;

  tmp->SetupFrom(m_algo.get());
  m_algo = std::move(tmp);

  return false;
}

void* Context::SettingsHandler_ReadOpen(ImGuiContext*,
                                        ImGuiSettingsHandler* handler,
                                        const char* name) {
  CELLNTA_PROFILE;

  Context* ctx = (Context*)handler->UserData;
  ctx->m_firstStartup = false;
  ctx->m_currentWindow.clear();
  return (void*)ctx;
}

void Context::SettingsHandler_ReadLine(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       void* entry, const char* line) {
  CELLNTA_PROFILE;

  Context* ctx = (Context*)entry;

  if (line[0] == '(') {
    size_t size = std::strlen(line);
    if (line[size - 1] == ')') {
      ctx->m_currentWindow = std::string(line + 1, size - 2);
    }
  } else if (!ctx->m_currentWindow.empty()) {
    for (auto& window : ctx->m_windowsData) {
      if (ctx->m_currentWindow == window->GetProperties().Name) {
        window->ReadProperties(line);
      }
    }
  }
}

void Context::SettingsHandler_WriteAll(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       ImGuiTextBuffer* buf) {
  CELLNTA_PROFILE;

  if (handler->UserData == nullptr)
    return;

  Context* ctx = (Context*)handler->UserData;
  buf->reserve(buf->size() + 6 * 6);
  buf->appendf("[%s][%s]\n", "CellNta", "Context");
  ctx->WriteWindowProperties(handler, buf);
  buf->append("\n");
}

void Context::WriteWindowProperties(ImGuiSettingsHandler* handler,
                                    ImGuiTextBuffer* buf) {
  CELLNTA_PROFILE;

  for (auto& window : m_windowsData) {
    buf->appendf("(%s)\n", window->GetProperties().Name);
    window->WriteProperties(buf);
    buf->appendf("\n");
  }
}

char* Context::GetTmpBuffer() {
  ImGuiContext& g = *GImGui;
  return g.TempBuffer.Data;
}

char* Context::GetTmpBuffer(size_t& size) {
  ImGuiContext& g = *GImGui;
  // Need return specified size
  // if (size != 0)
  //   if (size > g.TempBuffer.Capacity)
  //     g.TempBuffer.resize(size);
  size = g.TempBuffer.Size;
  return g.TempBuffer.Data;
}
