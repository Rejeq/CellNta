option(CELLNTA_DRAWER_GL "Enable OpenGL renderer backend" ON)

add_library(CellntaRenderer STATIC
  ./Camera3d.cpp
  ./CameraController.cpp
  ./CameraNd.cpp
  ./ColorStorage.cpp
  ./DrawerBackend.cpp
  ./HypercubeStorage.cpp
  ./NCellStorage.cpp
  ./RenderData.cpp
  ./Renderer.cpp
  ./ShuffleAxisStorage.cpp
  ./Transform.cpp
)
add_library(Cellnta::Renderer ALIAS CellntaRenderer)

target_link_libraries(CellntaRenderer PUBLIC
  Cellnta::Base
)

set(DrawerBackDef "")

if (${CELLNTA_DRAWER_GL})
  list(APPEND DrawerBackDef CELLNTA_DRAWER_GL=1)
  add_subdirectory(./Drawer/GL/)
endif()

target_compile_definitions(CellntaRenderer PUBLIC ${DrawerBackDef})
