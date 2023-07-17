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

  ./src/Camera/3dAction.cpp
  ./src/Camera/3dView.cpp
  ./src/Camera/ControllerView.cpp
  ./src/Camera/NdAction.cpp
  ./src/Camera/NdView.cpp
  ./src/Camera/Window.cpp

  ./src/Menubar/AppearanceAction.cpp
  ./src/Menubar/Window.cpp

  ./src/Pattern/Action.cpp
  ./src/Pattern/Window.cpp

  ./src/Renderer/HypercubeAction.cpp
  ./src/Renderer/HypercubeView.cpp
  ./src/Renderer/RenderDataAction.cpp
  ./src/Renderer/RenderDataView.cpp
  ./src/Renderer/Window.cpp

  ./src/Scene/FocusAction.cpp
  ./src/Scene/Window.cpp

  ./src/World/Action.cpp
  ./src/World/RandomAction.cpp
  ./src/World/RandomView.cpp
  ./src/World/RuleAction.cpp
  ./src/World/SimpleAction.cpp
  ./src/World/SimpleView.cpp
  ./src/World/Window.cpp

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
  Cellnta::Drawer::GL
  ImGui::ImGui
  SDL2::SDL2
  ${CMAKE_DL_LIBS}
)

if(COMMAND target_precompile_headers AND CELLNTA_PCH)
  configure_file(${CELLNTA_DIR}/cmake/DesktopPch.h.in ${CMAKE_CURRENT_BINARY_DIR}/DesktopPch.h @ONLY)
  target_precompile_headers(CellntaDesktop PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/DesktopPch.h)
endif()
