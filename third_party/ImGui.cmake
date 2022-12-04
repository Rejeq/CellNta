set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
set(IMGUI_BACKEND_DIR ${IMGUI_DIR}/backends)
set(IMGUI_MISC_DIR ${IMGUI_DIR}/misc)

function(add_imgui_library TARGET)
  cmake_parse_arguments(TARGET "" "" "HEADERS;SOURCES;DEPS" ${ARGN})

  add_library(${TARGET} OBJECT)
  add_library(ImGui::${TARGET} ALIAS ${TARGET})

  target_sources(${TARGET} PRIVATE ${TARGET_SOURCES})
  target_include_directories(${TARGET} PUBLIC ${TARGET_HEADERS})
  target_link_libraries(${TARGET} PRIVATE ImGui::ImGui ${TARGET_DEPS})

  # Prevent clang-tidy checks if enabled
  set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "")
endfunction()

add_imgui_library(ImGui SOURCES
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui.cpp
  HEADERS ${IMGUI_DIR}
)

add_imgui_library(ImplSDL2
  SOURCES ${IMGUI_BACKEND_DIR}/imgui_impl_sdl.cpp
  HEADERS ${IMGUI_BACKEND_DIR} ${SDL2_INCLUDE_DIRS}
  DEPS ${SDL2_LIBRARIES}
)

add_imgui_library(ImplOpenGL3
  SOURCES ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp
  HEADERS ${IMGUI_BACKEND_DIR}
  DEPS OpenGL::GL
)
add_imgui_library(MiscCpp
  SOURCES ${IMGUI_MISC_DIR}/cpp/imgui_stdlib.cpp
  HEADERS ${IMGUI_MISC_DIR}/cpp
)
