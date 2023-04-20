#pragma once

#include "Cellnta/Renderer/ColorStorage.h"

namespace Cellnta {

class ColorDrawer {
 public:
  virtual ~ColorDrawer() = default;

  virtual void Update(ColorStorage& color) = 0;
};

}  // namespace Cellnta

