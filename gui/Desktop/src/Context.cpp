#include "Context.h"

#include <memory>
#include <vector>

#include <Cellnta/Renderer/Camera3d.h>
#include <Cellnta/Renderer/CameraNd.h>
#include <Cellnta/Renderer/HypercubeStorage.h>
#include <Cellnta/Renderer/RenderData.h>
#include <Cellnta/World/WorldList.h>

#include "Log.h"

using namespace Ui;

static std::unique_ptr<Context> s_ctx;

Context::Context() {
  if (m_renderer.Init(DrawerBackend::GL)) {
    DESKTOP_LOG_CRITICAL("Unable to initialize renderer");
    return;
  }

  m_camera3d = std::make_shared<Cellnta::Camera3d>();
  if (m_camera3d == nullptr)
    DESKTOP_LOG_ERROR("Unable to create 3d camera");
  m_renderer.SetCamera3d(m_camera3d);

  m_cameraNd = std::make_shared<Cellnta::CameraNdList>();
  if (m_cameraNd == nullptr)
    DESKTOP_LOG_ERROR("Unable to create Nd camera");
  m_renderer.SetCameraNd(m_cameraNd);

  m_hypercube = std::make_shared<Cellnta::HypercubeStorage>();
  if (m_hypercube == nullptr)
    DESKTOP_LOG_ERROR("Unable to create hypercube");
  m_renderer.SetHypercube(m_hypercube);

  m_renderData = std::make_shared<Cellnta::RenderData>();
  if (m_renderData == nullptr)
    DESKTOP_LOG_ERROR("Unable to create hypercube");
  m_renderer.SetData(m_renderData);

  m_keybinds.SetupDefault();

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
  if (m_world != nullptr) {
    Cellnta::Rule newRule = m_world->GetRule().ChangeDimension(dim);
    m_world->SetRule(newRule);
  }
}

void Context::Update() {
  CELLNTA_PROFILE;

  ProccessKeybindings();

  m_renderer.Update();

  Cellnta::RenderData* data = m_renderer.GetData();
  if (data == nullptr)
    return;

  if (data->DesireArea()) {
    if (m_world == nullptr)
      DESKTOP_LOG_ERROR(
          "Unable to update world in RenderData: m_world was nullptr");
    else
      data->Update(*m_world);
    data->DesireAreaProcessed();
  }
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

  m_mouseDeltaX = 0;
  m_mouseDeltaY = 0;
  ImGui::PopID();
}

void Context::PushAction(Action::BasePtr&& action) {
  action->SetContext(this);
  action->Execute();

  if (action->CanUndo()) {
    m_undoRedo.PushAction(std::move(action));
  }
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

bool Context::SetWorld(const Cellnta::WorldType type) {
  CELLNTA_PROFILE;

  std::unique_ptr<Cellnta::World> tmp = Cellnta::CreateWorldInstance(type);
  if (tmp == nullptr) {
    DESKTOP_LOG_ERROR(
        "Unable to create new world instance when changing world");
    return true;
  }

  if (m_world != nullptr)
    tmp->SetupFrom(*m_world);
  m_world = std::move(tmp);

  Cellnta::RenderData* data = m_renderer.GetData();
  if (data == nullptr) {
    DESKTOP_LOG_WARN("Unable to clear RenderData in when changing world");
    return false;
  }

  data->GetCells().Clear();
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

void Context::ProccessKeybindings() {
  CELLNTA_PROFILE;

  const auto& io = ImGui::GetIO();
  if (io.WantTextInput)
    return;

  Keymap map;
  map.mod |= (io.KeyCtrl) ? ImGuiMod_Ctrl : 0;
  map.mod |= (io.KeyShift) ? ImGuiMod_Shift : 0;
  map.mod |= (io.KeyAlt) ? ImGuiMod_Alt : 0;
  map.mod |= (io.KeySuper) ? ImGuiMod_Super : 0;

  for (int key = ImGuiKey_None; key < ImGuiKey_COUNT; ++key) {
    if (!ImGui::IsKeyPressed((ImGuiKey)key))
      continue;

    map.key = (ImGuiKey)key;
    auto res = m_keybinds.Find(map);
    if (res == nullptr)
      continue;

    if (!res)
      DESKTOP_LOG_WARN("Key bind has a ill formed function, skip it");
    else
      res(*this);
  }
}
