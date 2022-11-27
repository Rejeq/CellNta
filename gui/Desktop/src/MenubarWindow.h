#pragma once

#include <functional>

#include "Window.h"

namespace Ui {

class MenubarWindow : public Window
{
  static constexpr WindowProperties DefaultProperties = {
    .Name = "Menubar",
    .Opened = true,
    .VisibleInMenubar = false,
  };

public:
  MenubarWindow(const std::function<void(const Context&)>& onResetLayout)
    : Window(DefaultProperties), m_OnResetLayout(onResetLayout) {}

  void Draw() override;

private:
  void DrawWindowsButtons();
  float DrawFramerate(float right);
  float DrawResetLayout(float right);

  std::function<void(const Context&)> m_OnResetLayout;
  float m_frameratePos = -1.0f;
  float m_layoutPos = -1.0f;
};

} //namespace Ui
