cmake_minimum_required(VERSION 3.16)

project(CellNta)

option(ENABLE_PROFILING "Enable profiling" OFF)

set(CXX_ACTIVE_STANDART cxx_std_17)

set(CELLNTA_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CELLNTA_THIRD_PARTY "${CELLNTA_DIR}/third_party")

# Libraries directories
set(TRACY_DIR ${CELLNTA_THIRD_PARTY}/tracy)
list(APPEND CELLNTA_INC
  "${CELLNTA_THIRD_PARTY}/glew/include"
  "${CELLNTA_THIRD_PARTY}/Eigen"
  "${CELLNTA_THIRD_PARTY}/boost"
  "${CELLNTA_THIRD_PARTY}/spdlog"
)

set(CELLNTA_FOLDERS
  "." #Base
  "Algorithms"
  "Renderer"
)

add_library(CellNta STATIC)
target_compile_features(CellNta PRIVATE ${CXX_ACTIVE_STANDART})

list(APPEND CELLNTA_INC "${CELLNTA_DIR}/include")

foreach(FOLDER ${CELLNTA_FOLDERS})
  file(GLOB FOLDER_SRC
    "${CELLNTA_DIR}/include/Cellnta/${FOLDER}/*.h"
    "${CELLNTA_DIR}/src/${FOLDER}/*.cpp"
  )
  list(APPEND CELLNTA_SRC ${FOLDER_SRC})

  if(FOLDER STREQUAL  ".")
    set(FOLDER "Base")
  endif()
  source_group("${FOLDER}" FILES ${FOLDER_SRC})
endforeach()

if(ENABLE_PROFILING)
  message("Profiling enabled")
  list(APPEND CELLNTA_INC ${TRACY_DIR}/tracy)
  list(APPEND CELLNTA_SRC ${TRACY_DIR}/TracyClient.cpp)
  target_compile_definitions(CellNta PUBLIC
    CELLNTA_PROFILING_ENABLED TRACY_ENABLE)

  if(UNIX)
    target_link_libraries(CellNta PUBLIC pthread)
  endif()
endif()

if(MSVC)
  target_compile_options(CellNta PRIVATE
    $<$<CONFIG:DEBUG>:/W4 /wd4244 /wd4267 /wd4127>)
else()
  target_compile_options(CellNta PRIVATE
    $<$<CONFIG:DEBUG>:-Wall -Wextra>)
endif()

target_sources(CellNta PRIVATE ${CELLNTA_SRC})
target_include_directories(CellNta PUBLIC ${CELLNTA_INC})

include("${CELLNTA_DIR}/gui/Gui.cmake")
