#include "Cellnta/Renderer/Drawer/GL/Instance.h"
#include "Cellnta/Renderer/Drawer/GL/Scene.h"

#include "Cellnta/Log.h"

using namespace Cellnta;

DrawerPtr<SceneDrawer> InstanceDrawerGL::CreateScene() {
  return MakeDrawerInstance<SceneDrawerGL>();
}

