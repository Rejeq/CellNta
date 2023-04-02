cmake_minimum_required(VERSION 3.16)

project(Cellnta)

option(CELLNTA_PROFILING "Enable profiling" OFF)
option(CELLNTA_TIDY "Enable clang-tidy checks" OFF)
option(CELLNTA_LOGGING "Enable logging" ON)
option(CELLNTA_TRACE_LOGGING "Enable detailed logging. Causes performance issues" OFF)
option(CELLNTA_TESTS "Enable google test" OFF)
option(CELLNTA_FETCHING "Enable fetching third-party libraries" ON)
option(CELLNTA_PCH "Enable building using a precompiled header to speed up compilation time" ON)
option(CELLNTA_GUI "Enable building gui targets" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

set(CELLNTA_DIR ${CMAKE_CURRENT_LIST_DIR})
set(CELLNTA_THIRD_PARTY ${CELLNTA_DIR}/third_party)

list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if(CELLNTA_TIDY)
  find_program(CLANG_TIDY_BIN NAMES "clang-tidy" REQUIRED)

  set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_BIN})
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  message(STATUS "Enabled clang-tidy: ${CLANG_TIDY_BIN}")
endif()

if(CELLNTA_TESTS)
  if(CELLNTA_LOGGING)
    set(CELLNTA_LOGGING OFF)
    message(WARNING "Logging will be disabled due to enabled tests")
  endif()

  enable_testing()
  add_subdirectory(tests)
  message(STATUS "Tests enabled")
endif()

add_subdirectory(src)

if(CELLNTA_GUI)
  add_subdirectory(gui)
endif()
