find_package(OpenGL REQUIRED)
find_package(GLEW)

add_library(CellntaRenderer STATIC
  ./Camera3d.cpp
  ./CameraController.cpp
  ./CameraNd.cpp
  ./ColorStorage.cpp
  ./GlBackend.cpp
  ./HypercubeStorage.cpp
  ./NCellStorage.cpp
  ./RenderData.cpp
  ./Renderer.cpp
  ./Shader.cpp
  ./Transform.cpp

  ./Drawer/Color.cpp
  ./Drawer/Hypercube.cpp
  ./Drawer/NCell.cpp
)
add_library(Cellnta::Renderer ALIAS CellntaRenderer)

target_link_libraries(CellntaRenderer PUBLIC
  Cellnta::Base
  OpenGL::GL
  GLEW::GLEW
)
