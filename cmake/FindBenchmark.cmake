find_package(GTest)

list(REMOVE_ITEM CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")
find_package(benchmark QUIET)
list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if (NOT BENCHMARK_FOUND AND CELLNTA_FETCHING)
  set(BENCHMARK_URL "https://github.com/google/benchmark/archive/refs/tags/v1.8.0.tar.gz")
  message(STATUS "Loading benchmark from ${BENCHMARK_URL}")

  set(BENCHMARK_DOWNLOAD_DEPENDENCIES OFF CACHE BOOL "")
  set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "")

  include(FetchContent)
  FetchContent_Declare(benchmark
    URL ${BENCHMARK_URL}
  )

  FetchContent_MakeAvailable(benchmark)
  message(STATUS "benchmark configured successfully")
endif()

