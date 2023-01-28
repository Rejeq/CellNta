option(CELLNTA_GUI_DESKTOP "ImGui ui" ON)

if(CELLNTA_GUI_DESKTOP)
  message(STATUS "Building desktop gui")
  add_subdirectory(Desktop)
endif()

