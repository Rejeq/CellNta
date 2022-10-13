#pragma once

#include <cfloat>

#include <string>
#include <iomanip>
#include <vector>

#include <Eigen/Core>
#include <imgui.h>
#include <imgui_internal.h>

#include <Canvas.h>

#include "Window.h"

namespace Ui
{
  class Context
  {
  public:
    Context(std::unique_ptr<Canvas>&& canvas);

    void AddWindow(std::unique_ptr<Window>&& window);

    void Draw();

    uint32_t SceneTextureId = 0;
    bool SceneWindowFocused = false;

    ImGuiID MainDockspaceId = 0;
    bool FirstStartup = true;

    Window* GetWindowByName(const char* name);
    Canvas& GetCanvas() { return *m_canvas; }
    const Canvas& GetCanvas() const { return *m_canvas; }

    char* GetTmpBuffer();
    char* GetTmpBuffer(size_t& size);

  private:
    float DrawFramerate(const float& offset);
    void DrawScene();
    void ResetLayout();


    void ReadWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
    void WriteWindowProperties(ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);

    static void  SettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf);
    static void  SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler* handler, void* entry, const char* line);
    static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name);

    std::vector<std::unique_ptr<Window>> m_windowsData;
    std::unique_ptr<Canvas> m_canvas = nullptr;

    std::string m_currentWindow;
  };
}
