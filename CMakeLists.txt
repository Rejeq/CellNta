cmake_minimum_required(VERSION 3.16)

if (POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

project(Cellnta)

option(CELLNTA_PROFILING "Enable profiling" OFF)
option(CELLNTA_TIDY "Enable clang-tidy checks" OFF)
option(CELLNTA_LOGGING "Enable logging" ON)
option(CELLNTA_TRACE_LOGGING "Enable detailed logging. Causes performance issues" OFF)
option(CELLNTA_TESTS "Enable google test" OFF)
option(CELLNTA_FETCHING "Enable fetching third-party libraries" ON)
option(CELLNTA_PCH "Enable building using a precompiled header to speed up compilation time" ON)
option(CELLNTA_GUI "Enable building gui targets" ON)
option(CELLNTA_BENCHS "Enable benchmarks" OFF)
option(CELLNTA_OPENMP "Enable concurrency" ON)

# This is required for cmake-gui, in cli its already specified as Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, Release or Debug" FORCE)
  message(STATUS "Build type was set to '${CMAKE_BUILD_TYPE}' because is was not specified")
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
  enable_testing()
  add_subdirectory(tests)
  message(STATUS "Tests enabled")
endif()

if(CELLNTA_BENCHS)
  add_subdirectory(benchs)
  message(STATUS "Benchmarking enabled")
endif()

add_subdirectory(src)

if(CELLNTA_GUI)
  add_subdirectory(gui)
endif()
