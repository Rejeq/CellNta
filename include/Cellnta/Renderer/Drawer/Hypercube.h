#pragma once

#include "Cellnta/Renderer/Drawer/Color.h"
#include "Cellnta/Renderer/HypercubeStorage.h"

namespace Cellnta {

class HypercubeDrawer {
 public:
  virtual ~HypercubeDrawer() = default;

  virtual void UpdatePoints(const HypercubeStorage& cube) = 0;
  virtual void UpdateIndices(HypercubeStorage& cube) = 0;
  virtual void UpdateColor(HypercubeStorage& cube) = 0;

  virtual const ColorDrawer& GetColor() const = 0;
  virtual ColorDrawer& GetColor() = 0;
};

}  // namespace Cellnta
