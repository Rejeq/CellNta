#pragma once

#include <memory>

#include "Cellnta/Renderer/Drawer/Scene.h"

#if CELLNTA_DRAWER_GL

#include "Cellnta/Renderer/Drawer/GL/GL.h"
#include "Cellnta/Renderer/Drawer/GL/Scene.h"

#endif

enum class DrawerBackend {
#if CELLNTA_DRAWER_GL
  GL = 1,
#endif
};

namespace Cellnta {

std::unique_ptr<SceneDrawer> CreateSceneDrawerInstance(DrawerBackend backend);

}  // namespace Cellnta
