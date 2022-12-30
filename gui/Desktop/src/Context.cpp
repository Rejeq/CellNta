#include "Context.h"

#include <memory>
#include <vector>

#include <Cellnta/Log.h>
#include <Cellnta/Algorithms/AlgoList.h>
#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/HypercubeStorage.h>

#include "Widgets.h"

using namespace Ui;

static std::unique_ptr<Context> s_ctx;

Context::Context() {
  m_camera3d = std::make_shared<Cellnta::Camera3d>();
  if (m_camera3d == nullptr)
    CELLNTA_LOG_ERROR("Unable to create 3d camera");
  m_renderer.SetCamera3d(m_camera3d);

  m_cameraNd = std::make_shared<Cellnta::CameraNdList>();
  if (m_cameraNd == nullptr)
    CELLNTA_LOG_ERROR("Unable to create Nd camera");
  m_renderer.SetCameraNd(m_cameraNd);

  m_hypercube = std::make_shared<Cellnta::HypercubeStorage>();
  if (m_hypercube == nullptr)
    CELLNTA_LOG_ERROR("Unable to create hypercube");
  m_renderer.SetHypercube(m_hypercube);

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

  Cellnta::RenderData* data = m_renderer.GetData();
  if (data == nullptr)
    return;

  if (m_algo->NeedLoadWorld() || data->DesireArea())
    m_algo->LoadWorld(data);

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

  Cellnta::RenderData* data = m_renderer.GetData();
  if (data != nullptr)
    data->GetCells().clear();

  return false;
}

void* Context::SettingsHandler_ReadOpen(ImGuiContext* /*unused*/,
                                        ImGuiSettingsHandler* handler,
                                        const char* /*unused*/) {
  CELLNTA_PROFILE;

  auto* ctx = (Context*)handler->UserData;
  ctx->m_firstStartup = false;
  ctx->m_currentWindow.clear();
  return (void*)ctx;
}

void Context::SettingsHandler_ReadLine(ImGuiContext* /*unused*/,
                                       ImGuiSettingsHandler* /*unused*/,
                                       void* entry, const char* line) {
  CELLNTA_PROFILE;

  auto* ctx = (Context*)entry;

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

void Context::SettingsHandler_WriteAll(ImGuiContext* /*unused*/,
                                       ImGuiSettingsHandler* handler,
                                       ImGuiTextBuffer* buf) {
  CELLNTA_PROFILE;

  if (handler->UserData == nullptr)
    return;

  auto* ctx = (Context*)handler->UserData;
  buf->reserve(buf->size() + 6 * 6);
  buf->appendf("[%s][%s]\n", "CellNta", "Context");
  ctx->WriteWindowProperties(handler, buf);
  buf->append("\n");
}

void Context::WriteWindowProperties(ImGuiSettingsHandler* /*unused*/,
                                    ImGuiTextBuffer* buf) {
  CELLNTA_PROFILE;

  for (auto& window : m_windowsData) {
    buf->appendf("(%s)\n", window->GetProperties().Name);
    window->WriteProperties(buf);
    buf->appendf("\n");
  }
}
