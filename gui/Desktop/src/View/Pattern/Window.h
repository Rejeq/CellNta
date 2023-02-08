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

  void SetPosition(const Cellnta::Cell::Pos& pos) { m_pos = pos; }
  Cellnta::Cell::Pos GetPosition() const { return m_pos; }
 private:
  Cellnta::Cell::Pos m_pos;
};

}  // namespace Ui
