set(GUI_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RESOURCE_DIR ${CELLNTA_DIR}/Resource/)


set(IMGUI_CONFIG ${GUI_DIR}/src/imgui_config.h)

find_package(SDL2 REQUIRED)
include(${CELLNTA_THIRD_PARTY}/ImGui.cmake)




file(GLOB SOURCE_LIST
  ${GUI_DIR}/src/*.h
  ${GUI_DIR}/src/*.cpp
)

add_executable(GUI ${SOURCE_LIST})
target_compile_features(GUI PRIVATE cxx_std_17)
add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
add_compile_options("$<$<CONFIG:DEBUG>:-O0 -g3>")

target_compile_definitions(GUI PUBLIC
  RESOURCE_DIR="${RESOURCE_DIR}"
  IMGUI_USER_CONFIG="${IMGUI_CONFIG}"
)

target_include_directories(GUI PUBLIC
  ${SDL2_INCLUDE_DIRS}
)

target_link_libraries(GUI PUBLIC
  CellNta
  ImGui::ImGui
  ImGui::ImplOpenGL3
  ImGui::ImplSDL2
  ImGui::MiscCpp
  ${SDL2_LIBRARIES}
)
