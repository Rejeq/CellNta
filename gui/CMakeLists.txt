option(GUI_DESKTOP "ImGui ui" ON)

if(GUI_DESKTOP)
  message(STATUS "Building desktop gui")
  add_subdirectory(Desktop)
endif()

