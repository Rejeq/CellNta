#pragma once

#include <Cellnta/Renderer/Camera3d.h>

#include "Context.h"

namespace Ui {

class Camera3dView {
 public:
  static void DrawOptions(Context& ctx, Cellnta::Camera3d& cam);
  static void Draw(Context& ctx, Cellnta::Camera3d& cam);
};

}  // namespace Ui
