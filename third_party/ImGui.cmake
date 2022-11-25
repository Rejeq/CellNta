set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)
set(IMGUI_BACKEND_DIR ${IMGUI_DIR}/backends)
set(IMGUI_MISC_DIR ${IMGUI_DIR}/misc)

add_library(ImGui::ImGui INTERFACE IMPORTED)
target_sources(ImGui::ImGui INTERFACE
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui.cpp)
target_include_directories(ImGui::ImGui INTERFACE
  ${IMGUI_DIR})

add_library(ImGui::ImplOpenGL3 INTERFACE IMPORTED)
target_sources(ImGui::ImplOpenGL3 INTERFACE
  ${IMGUI_BACKEND_DIR}/imgui_impl_opengl3.cpp)
target_include_directories(ImGui::ImplOpenGL3 INTERFACE
  ${IMGUI_BACKEND_DIR})

add_library(ImGui::ImplSDL2 INTERFACE IMPORTED)
target_sources(ImGui::ImplSDL2 INTERFACE
  ${IMGUI_BACKEND_DIR}/imgui_impl_sdl.cpp)
target_include_directories(ImGui::ImplSDL2 INTERFACE
  ${IMGUI_BACKEND_DIR})

add_library(ImGui::MiscCpp INTERFACE IMPORTED)
target_sources(ImGui::MiscCpp INTERFACE
    ${IMGUI_MISC_DIR}/cpp/imgui_stdlib.cpp)
target_include_directories(ImGui::MiscCpp INTERFACE
    ${IMGUI_MISC_DIR}/cpp)
