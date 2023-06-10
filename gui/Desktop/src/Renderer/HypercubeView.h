#pragma once

#include <Cellnta/Renderer/HypercubeStorage.h>

#include "Context.h"

namespace Ui {

class HypercubeView {
 public:
  static void Draw(Context& ctx, Cellnta::HypercubeStorage& cube);
};

}  // namespace Ui
