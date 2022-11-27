#pragma once

#include <cfloat>

#include <string>
#include <iomanip>
#include <vector>

#include <imgui.h>
#include <imgui_internal.h>

#include <Cellnta/Algorithms/AlgoBase.h>
#include <Cellnta/Renderer/Renderer.h>

#include "Window.h"

namespace Ui {

class Context
{
  using WindowsDataType = std::vector<std::unique_ptr<Window>>;

public:
  Context();

  void AddWindow(std::unique_ptr<Window>&& window);
  void SetDimension(int dim);

  void Update();
  void Draw();

  void SetupDockspace();
  ImGuiID GetDockspace() const { return m_dockspaceId; }

  const WindowsDataType& GetWindows() const { return m_windowsData; }
  Window* GetWindowByName(const std::string& name);

  bool SetAlgo(const Cellnta::AlgoType type);
  const Cellnta::AlgoBase& GetAlgo() const { return *m_algo; }
  Cellnta::AlgoBase& GetAlgo() { return *m_algo; }

  const Cellnta::Renderer& GetRenderer() const { return m_renderer; }
  Cellnta::Renderer& GetRenderer() { return m_renderer; }


  void SetOnFirstStartup(const std::function<void(const Context&)>& onFirstStartup){
    m_OnFirstStartup = onFirstStartup;
  }

  char* GetTmpBuffer();
  char* GetTmpBuffer(size_t& size);

private:
  void ReadWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
  void WriteWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

  static void  SettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
  static void  SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line);
  static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);


  WindowsDataType m_windowsData;
	std::unique_ptr<Cellnta::AlgoBase> m_algo;
  Cellnta::Renderer m_renderer;

  std::string m_currentWindow;

  std::function<void(const Context&)> m_OnFirstStartup;

  bool m_firstStartup = true;
  ImGuiID m_dockspaceId = 0;
};

} //namespace Ui
