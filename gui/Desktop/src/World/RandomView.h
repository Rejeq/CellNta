#pragma once

#include <Cellnta/World/Impl/Random.h>

#include "Context.h"

namespace Ui {

class WorldRandomView {
 public:
  static void Draw(Context& ctx, Cellnta::WorldImplRandom& world);
};

}  // namespace Ui
