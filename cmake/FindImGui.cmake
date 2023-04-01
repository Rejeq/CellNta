if (NOT IMGUI_DIR AND CELLNTA_FETCHING)
  set(IMGUI_URL  "https://github.com/ocornut/imgui/archive/d822c65317ba881798bed8fce9ffba267d27dada.tar.gz")
  set(IMGUI_VERSION "v1.89.2 - docking branch")
  message(STATUS "Loading ImGui from ${IMGUI_URL} (${IMGUI_VERSION})")

  include(FetchContent)
  FetchContent_Declare(ImGui
    URL ${IMGUI_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
  )
  FetchContent_MakeAvailable(ImGui)
  set(IMGUI_DIR ${imgui_SOURCE_DIR})
endif()

set(IMGUI_BACKEND_DIR ${IMGUI_DIR}/backends)
set(IMGUI_MISC_DIR ${IMGUI_DIR}/misc)

function(add_imgui_library)
  cmake_parse_arguments(TARGET "" "" "HEADERS;SOURCES;DEPS" ${ARGN})

  target_sources(ImGui PRIVATE ${TARGET_SOURCES})
  target_include_directories(ImGui PUBLIC ${TARGET_HEADERS})
  target_link_libraries(ImGui PRIVATE ImGui::ImGui ${TARGET_DEPS})
endfunction()

add_library(ImGui OBJECT)
add_library(ImGui::ImGui ALIAS ImGui)

# Prevent clang-tidy checks if enabled
set_target_properties(ImGui PROPERTIES CXX_CLANG_TIDY "")

add_imgui_library(ImGui SOURCES
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui.cpp
  HEADERS ${IMGUI_DIR}
)

add_imgui_library(
  SOURCES ${IMGUI_BACKEND_DIR}/imgui_impl_sdl.cpp
  HEADERS ${IMGUI_BACKEND_DIR} ${SDL2_INCLUDE_DIRS}
  DEPS SDL2::SDL2
)

find_package(OpenGL)
add_imgui_library(
  SOURCES ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
  HEADERS ${IMGUI_BACKEND_DIR}
  DEPS OpenGL::GL
)
add_imgui_library(
  SOURCES ${IMGUI_MISC_DIR}/cpp/imgui_stdlib.cpp
  HEADERS ${IMGUI_MISC_DIR}/cpp
)

message(STATUS "ImGui configured successfully")
