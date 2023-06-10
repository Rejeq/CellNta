#pragma once

#include "Renderer/RenderDataView.h"
#include "WindowBase.h"

namespace Ui {

class RendererWindow : public Window, private RenderDataView {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Renderer",
  };

 public:
  RendererWindow() : Window(DefaultProperties) {}

  void Draw() override;
};

}  // namespace Ui
