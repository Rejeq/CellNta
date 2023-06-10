#pragma once

#include <Cellnta/World/Impl/Simple.h>

#include "Context.h"

namespace Ui {

class WorldSimpleView {
 public:
  static void Draw(Context& ctx, Cellnta::WorldImplSimple& world);
};

}  // namespace Ui
