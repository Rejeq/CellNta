find_package(OpenGL REQUIRED)
find_package(GLEW)

add_library(CellntaDrawerGL STATIC
  ./Color.cpp
  ./GL.cpp
  ./Hypercube.cpp
  ./NCell.cpp
  ./Scene.cpp
  ./Shader.cpp
)
add_library(Cellnta::Drawer::GL ALIAS CellntaDrawerGL)

target_link_libraries(CellntaDrawerGL PUBLIC
  Cellnta::Renderer
  OpenGL::GL
  GLEW::GLEW
)
