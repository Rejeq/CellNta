#pragma once

#include "Cellnta/Renderer/Drawer/Instance.h"
#include "Cellnta/Renderer/Drawer/Scene.h"

namespace Cellnta {

class InstanceDrawerGL {
 public:
  static DrawerPtr<SceneDrawer> CreateScene();
};

}  // namespace Cellnta
