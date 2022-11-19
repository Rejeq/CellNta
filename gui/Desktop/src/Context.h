#pragma once

#include <cfloat>

#include <string>
#include <iomanip>
#include <vector>

#include <Eigen/Core>
#include <imgui.h>
#include <imgui_internal.h>

#include <Cellnta/Canvas.h>

#include "Window.h"

namespace Ui {

class Context
{
  using WindowsDataType = std::vector<std::unique_ptr<Window>>;

public:
  Context();

  void AddWindow(std::unique_ptr<Window>&& window);

  void Draw();

  ImGuiID MainDockspaceId = 0;

  const WindowsDataType& GetWindows() const { return m_windowsData; }
  Window* GetWindowByName(const std::string& name);

  void SetCanvas(std::unique_ptr<Cellnta::Canvas>&& canvas) { m_canvas = std::move(canvas); }
  Cellnta::Canvas& GetCanvas() { return *m_canvas; }
  const Cellnta::Canvas& GetCanvas() const { return *m_canvas; }

  char* GetTmpBuffer();
  char* GetTmpBuffer(size_t& size);

  void SetOnFirstStartup(const std::function<void(const Context&)>& onFirstStartup){
    m_OnFirstStartup = onFirstStartup;
  }

private:
  void ReadWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
  void WriteWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

  static void  SettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
  static void  SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line);
  static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

  WindowsDataType m_windowsData;
  std::unique_ptr<Cellnta::Canvas> m_canvas = nullptr;

  std::string m_currentWindow;

  std::function<void(const Context&)> m_OnFirstStartup;

  bool m_firstStartup = true;
};

} //namespace Ui
