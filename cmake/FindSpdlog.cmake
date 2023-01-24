find_package(spdlog 1.11.0 CONFIG)

if (NOT spdlog_FOUND AND CELLNTA_FETCHING)
  set(SPDLOG_URL "https://github.com/gabime/spdlog/archive/refs/tags/v1.11.0.tar.gz")
  message(STATUS "Loading spdlog from ${SPDLOG_URL}")

  include(FetchContent)
  FetchContent_Declare(spdlog
    URL ${SPDLOG_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
  )
  FetchContent_MakeAvailable(spdlog)

  message(STATUS "spdlog configured successfully")
endif()
