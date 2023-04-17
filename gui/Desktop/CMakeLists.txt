set(GUI_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RESOURCE_DIR ${CELLNTA_DIR}/Resource/)


set(IMGUI_CONFIG ${GUI_DIR}/src/imgui_config.h)

find_package(SDL2 REQUIRED)
find_package(ImGui REQUIRED)


add_executable(CellntaDesktop
  ./src/Context.cpp
  ./src/Keybind.cpp
  ./src/Log.cpp
  ./src/Source.cpp
  ./src/UndoRedo.cpp

  ./src/View/Camera/Action.cpp
  ./src/View/Camera/Window.cpp
  ./src/View/Menubar/Action.cpp
  ./src/View/Menubar/Window.cpp
  ./src/View/Pattern/Action.cpp
  ./src/View/Pattern/Window.cpp
  ./src/View/Renderer/Action.cpp
  ./src/View/Renderer/Window.cpp
  ./src/View/Scene/Action.cpp
  ./src/View/Scene/Window.cpp
  ./src/View/World/Action.cpp
  ./src/View/World/Window.cpp

  ./src/Widgets/RightAlign.cpp
  ./src/Widgets/Utils.cpp
)

target_compile_features(CellntaDesktop PRIVATE cxx_std_20)

target_compile_definitions(CellntaDesktop PUBLIC
  RESOURCE_DIR="${RESOURCE_DIR}"
  IMGUI_USER_CONFIG="${IMGUI_CONFIG}"
)

target_include_directories(CellntaDesktop PUBLIC
  ${GUI_DIR}/src
)

target_link_libraries(CellntaDesktop PUBLIC
  Cellnta::World
  Cellnta::Renderer
  ImGui::ImGui
  SDL2::SDL2
  ${CMAKE_DL_LIBS}
)

if(COMMAND target_precompile_headers AND CELLNTA_PCH)
  configure_file(${CELLNTA_DIR}/cmake/DesktopPch.h.in ${CMAKE_CURRENT_BINARY_DIR}/DesktopPch.h @ONLY)
  target_precompile_headers(CellntaDesktop PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/DesktopPch.h)
endif()
