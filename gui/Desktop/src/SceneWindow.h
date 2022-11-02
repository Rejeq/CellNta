#pragma once

#include "Window.h"

namespace Ui
{
  class SceneWindow : public Window
  {
    static constexpr WindowProperties DefaultProperties = {
      .Name = "Scene",
      .VisibleInMenubar = false,
    };
  public:
    SceneWindow() : Window(DefaultProperties) {}

    void Draw() override;

  private:

    ImVec2 m_prevWindowSize = ImVec2(0, 0);
  };
}
