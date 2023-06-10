#pragma once

#include "WindowBase.h"

namespace Ui {

class CameraWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "Camera",
  };

 public:
  CameraWindow() : Window(DefaultProperties) {}

  void Draw() override;
};

}  // namespace Ui
