#pragma once

#include <cfloat>

#include <iomanip>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include <Cellnta/Renderer/Renderer.h>

#include "WindowBase.h"

namespace Cellnta {

class World;
enum class WorldType;

}


namespace Ui {

class Context {
  using WindowsDataType = std::vector<std::unique_ptr<Window>>;

 public:
  Context();

  void AddWindow(std::unique_ptr<Window>&& window);
  void SetDimension(int dim);

  void NextGeneration();

  void Update();
  void Draw();

  void SetupDockspace();
  ImGuiID GetDockspace() const { return m_dockspaceId; }

  const WindowsDataType& GetWindows() const { return m_windowsData; }
  Window* GetWindowByName(const std::string& name);

  bool SetWorld(const Cellnta::WorldType type);
  const Cellnta::World& GetWorld() const { return *m_world; }
  Cellnta::World& GetWorld() { return *m_world; }

  const Cellnta::Renderer& GetRenderer() const { return m_renderer; }
  Cellnta::Renderer& GetRenderer() { return m_renderer; }

  void SetOnFirstStartup(
      const std::function<void(const Context&)>& onFirstStartup) {
    m_OnFirstStartup = onFirstStartup;
  }

 private:
  void ReadWindowProperties(ImGuiSettingsHandler* handler,
                            ImGuiTextBuffer* buf);
  void WriteWindowProperties(ImGuiSettingsHandler* handler,
                             ImGuiTextBuffer* buf);

  static void SettingsHandler_WriteAll(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       ImGuiTextBuffer* buf);
  static void SettingsHandler_ReadLine(ImGuiContext*,
                                       ImGuiSettingsHandler* handler,
                                       void* entry, const char* line);
  static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*,
                                        const char* name);

  WindowsDataType m_windowsData;
  Cellnta::Renderer m_renderer;
  std::unique_ptr<Cellnta::World> m_world;
  std::shared_ptr<Cellnta::Camera3d> m_camera3d;
  std::shared_ptr<Cellnta::CameraNdList> m_cameraNd;
  std::shared_ptr<Cellnta::HypercubeStorage> m_hypercube;
  std::shared_ptr<Cellnta::RenderData> m_renderData;

  std::string m_currentWindow;

  std::function<void(const Context&)> m_OnFirstStartup;

  bool m_firstStartup = true;
  ImGuiID m_dockspaceId = 0;
};

}  // namespace Ui
