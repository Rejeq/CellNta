#pragma once

#include "Cellnta/Renderer/Drawer/Instance.h"
#include "Cellnta/Renderer/Drawer/Scene.h"

enum class DrawerBackend {
#if CELLNTA_DRAWER_GL
  GL = 1,
#endif
};

namespace Cellnta {

DrawerPtr<SceneDrawer> CreateSceneDrawerInstance(DrawerBackend backend);

}  // namespace Cellnta
