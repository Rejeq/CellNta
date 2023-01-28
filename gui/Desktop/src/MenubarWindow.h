#pragma once

#include <functional>

#include "Window.h"
#include "Widgets/RightAlign.h"

namespace Ui {

class MenubarWindow : public Window {
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

  std::function<void(const Context&)> m_OnResetLayout;
  Widget::RightAlign m_rAlign;
};

}  // namespace Ui
