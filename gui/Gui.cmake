set(CURRENT_DIR "${CMAKE_CURRENT_LIST_DIR}")

option(GUI_DESKTOP ON)
set(GUI_DESKTOP ON)

if(GUI_DESKTOP)
  message("Building gui for desktop")
  add_subdirectory("${CURRENT_DIR}/Desktop")
endif()

