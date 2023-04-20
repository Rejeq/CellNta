#pragma once

#include "Cellnta/Renderer/NCellStorage.h"

namespace Cellnta {

class NCellDrawer {
 public:
  virtual void Update(const NCellStorage& cells) = 0;
};

}  // namespace Cellnta
