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
)

target_link_libraries(CellntaRenderer PUBLIC
  CellntaBase
  OpenGL::GL
  GLEW::GLEW
)
