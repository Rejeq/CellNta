#include "Cellnta/Renderer/DrawerBackend.h"

using namespace Cellnta;

std::unique_ptr<SceneDrawer> Cellnta::CreateSceneDrawerInstance(DrawerBackend backend) {
  switch (backend) {
#if CELLNTA_DRAWER_GL
    case DrawerBackend::GL:
      return std::make_unique<SceneDrawerGL>();
      break;
#endif
    default: break;
  }
  return nullptr;
}
