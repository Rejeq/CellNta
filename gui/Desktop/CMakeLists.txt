set(GUI_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(THIRD_PARTY "${GUI_DIR}/third_party")

set(SDL_DIR   "${THIRD_PARTY}/SDL2/")

set(RESOURCE_DIR "${CELLNTA_DIR}/Resource/")

include("${GUI_DIR}/ImGui.cmake")

set(IMGUI_CONFIG "${GUI_DIR}/src/imgui_config.h")
file(GLOB SOURCE_LIST
  ${GUI_DIR}/src/*.h
  ${GUI_DIR}/src/*.cpp
)

add_executable(GUI ${SOURCE_LIST})

target_compile_features(GUI PRIVATE ${CXX_ACTIVE_STANDART})

add_compile_options("$<$<CONFIG:RELEASE>:-O3>")
add_compile_options("$<$<CONFIG:DEBUG>:-g3 -O0>")

target_compile_definitions(GUI PUBLIC
  RESOURCE_DIR="${RESOURCE_DIR}"
  IMGUI_USER_CONFIG="${IMGUI_CONFIG}"
)

target_include_directories(GUI PRIVATE
  ${SDL_DIR}/include
)

target_link_libraries(GUI PUBLIC
  CellNta imgui)

if(WIN32)
  target_link_libraries(GUI PUBLIC
    SDL2 SDL2main glew32 opengl32)
elseif(UNIX)
  target_link_libraries(GUI PUBLIC
    SDL2 GL GLEW dl)
endif()
