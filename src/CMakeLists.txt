find_package(Spdlog)
find_package(Eigen)

add_library(CellntaBase STATIC
  ./Adjustable.cpp
  ./Area.cpp
  ./Log.cpp
  ./LogBase.cpp
  ./Snapshot.cpp
)
add_library(Cellnta::Base ALIAS CellntaBase)

target_include_directories(CellntaBase PUBLIC
  ${CELLNTA_DIR}/include
)

target_link_libraries(CellntaBase PUBLIC
  Eigen3::Eigen
  spdlog::spdlog
)

target_compile_features(CellntaBase PUBLIC cxx_std_17)

target_compile_options(CellntaBase PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4244 /wd4267 /wd4127>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

if(COMMAND target_precompile_headers AND CELLNTA_PCH)
  configure_file(${CELLNTA_DIR}/cmake/CellntaPch.h.in ${CMAKE_CURRENT_BINARY_DIR}/CellntaPch.h @ONLY)
  target_precompile_headers(CellntaBase PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/CellntaPch.h)
  message(STATUS "Precompiled header enabled")
endif()

if(CELLNTA_PROFILING)
  find_package(Tracy REQUIRED)

  target_link_libraries(CellntaBase PUBLIC Tracy::Client)
  target_compile_definitions(CellntaBase PUBLIC CELLNTA_PROFILING_ENABLED)
  message(STATUS "Profiling enabled")
endif()

if (CELLNTA_LOGGING)
  target_compile_definitions(CellntaBase PUBLIC CELLNTA_LOGGING_ENABLED)
  message(STATUS "Logging enabled")
endif()

if(CELLNTA_TRACE_LOGGING)
  target_compile_definitions(CellntaBase PUBLIC CELLNTA_TRACE_LOGGING_ENABLED)
  message(STATUS "Detailed logging enabled")
endif()

add_subdirectory(World)
add_subdirectory(Renderer)
