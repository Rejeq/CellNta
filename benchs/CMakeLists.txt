find_package(Benchmark)

if(NOT (${CMAKE_BUILD_TYPE} STREQUAL "Release"))
  message(WARNING "Benchmarks built in non release mode. Timings may be affected")
endif()

if(CELLNTA_TRACE_LOGGING)
  message(WARNING "CELLNTA_TRACE_LOGGING are enabled. Timings may be affected")
endif()


add_subdirectory(src)
