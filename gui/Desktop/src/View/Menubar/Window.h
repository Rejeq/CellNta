#pragma once

#include <functional>

#include "Widgets/RightAlign.h"
#include "WindowBase.h"

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
  void DrawFileMenu();
  void DrawEditMenu();
  void DrawViewMenu();

  void DrawWindowsList();

  std::function<void(const Context&)> m_OnResetLayout;
  Widget::RightAlign m_rAlign;

  bool m_showFramerate = true;
};

}  // namespace Ui
