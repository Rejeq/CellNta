#pragma once

#include <Cellnta/Renderer/NCellStorage.h>
#include <Cellnta/Area.h>

#include "WindowBase.h"

namespace Ui {

class RendererWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Renderer",
  };

 public:
  RendererWindow() : Window(DefaultProperties) {}

  void Draw() override;

 private:
  void ShowCollatingInfo();
  void DrawLoadedCells();

  Cellnta::Cell m_selectedCell;
};

}  // namespace Ui
