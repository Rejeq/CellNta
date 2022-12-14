set(GUI_DIR ${CMAKE_CURRENT_LIST_DIR})
set(RESOURCE_DIR ${CELLNTA_DIR}/Resource/)


set(IMGUI_CONFIG ${GUI_DIR}/src/imgui_config.h)

find_package(SDL2 REQUIRED)
include(${CELLNTA_THIRD_PARTY}/ImGui.cmake)


file(GLOB_RECURSE SOURCE_LIST
  src/*.h
  src/*.cpp
)

add_executable(GUI ${SOURCE_LIST})
target_compile_features(GUI PRIVATE cxx_std_20)

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
  ${SDL2_LIBRARIES}
  ${CMAKE_DL_LIBS}
)
