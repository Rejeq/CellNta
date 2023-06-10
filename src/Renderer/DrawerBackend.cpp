#include "Cellnta/Renderer/DrawerBackend.h"

#if CELLNTA_DRAWER_GL

#include "Cellnta/Renderer/Drawer/GL/Instance.h"

#endif

using namespace Cellnta;

DrawerPtr<SceneDrawer> Cellnta::CreateSceneDrawerInstance(
    DrawerBackend backend) {
  switch (backend) {
#if CELLNTA_DRAWER_GL
    case DrawerBackend::GL: return InstanceDrawerGL::CreateScene(); break;
#endif
    default: break;
  }
  return nullptr;
}
