cmake_minimum_required(VERSION 3.16)

project(CellNta)

option(CELLNTA_PROFILING "Enable profiling" OFF)
option(CELLNTA_TIDY "Enable clang-tidy checks" OFF)
option(CELLNTA_TRACE_LOGGING "Enable detailed logging. Causes performance issues" OFF)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CELLNTA_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CELLNTA_THIRD_PARTY ${CELLNTA_DIR}/third_party)
set(CELLNTA_FOLDERS
  "." #Base
  "Algorithms"
  "Renderer"
)

if(CELLNTA_TIDY)
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy" REQUIRED)

  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  message(STATUS "Enabled clang-tidy: ${CLANG_TIDY_BIN}")
endif()

set(BOOST_INCLUDEDIR "${CELLNTA_THIRD_PARTY}/boost")

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
include(${CELLNTA_THIRD_PARTY}/Eigen.cmake)

if(CELLNTA_PROFILING)
  include(${CELLNTA_THIRD_PARTY}/Tracy.cmake)
endif()


add_library(CellNta STATIC)
target_compile_features(CellNta PRIVATE cxx_std_17)

target_compile_options(CellNta PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4267 /wd4127>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

foreach(folder ${CELLNTA_FOLDERS})
  file(GLOB folder_src
    include/Cellnta/${folder}/*.h
    src/${folder}/*.cpp
  )

  if(folder STREQUAL  ".")
    set(folder "Base")
  endif()

  list(APPEND CELLNTA_SRC ${folder_src})
  source_group("${folder}" FILES ${folder_src})
endforeach()

target_sources(CellNta PRIVATE ${CELLNTA_SRC})
target_include_directories(CellNta PUBLIC include)

target_link_libraries(CellNta PUBLIC
  OpenGL::GL
  GLEW::GLEW
  Eigen3::Eigen
  Boost::boost
  spdlog::spdlog
)

if(CELLNTA_PROFILING_ENABLED)
  target_link_libraries(CellNta PUBLIC Tracy::Client)
  target_compile_definitions(CellNta PUBLIC CELLNTA_PROFILING_ENABLED)
  message(STATUS "Profiling enabled")
endif()

if(CELLNTA_TRACE_LOGGING)
  target_compile_definitions(CellNta PUBLIC CELLNTA_TRACE_LOGGING_ENABLED)
  message(STATUS "Detailed logging enabled")
endif()

add_subdirectory(gui)
