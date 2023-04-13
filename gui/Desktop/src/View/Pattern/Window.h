#pragma once

#include <Cellnta/Area.h>

#include "WindowBase.h"

namespace Ui {

class PatternWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
    .Name = "Pattern",
    .VisibleInMenubar = true,
  };

 public:
  PatternWindow() : Window(DefaultProperties) {}

  void Draw() override;

 private:
  Cellnta::Cell::Pos m_pos;
};

}  // namespace Ui
