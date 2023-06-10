#pragma once

#include <Cellnta/Renderer/CameraController.h>

namespace Ui {

class CameraControllerView {
 public:
  enum class PressedType {
    NONE,
    USE_PERSPECTIVE,
  };

  static PressedType Draw(const Cellnta::CameraController& cont, bool* value);
};

}  // namespace Ui
