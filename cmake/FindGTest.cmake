list(REMOVE_ITEM CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")
find_package(GTest 1.11.0)
list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if (NOT GTEST_FOUND AND CELLNTA_FETCHING)
    set(GTEST_URL "https://github.com/google/googletest/archive/refs/tags/release-1.11.0.tar.gz")
    message(STATUS "Loading GTEST from ${GTEST_URL}")

  include(FetchContent)
  FetchContent_Declare(gtest
    URL ${GTEST_URL}
  )

FetchContent_MakeAvailable(gtest)
  message(STATUS "GTEST configured successfully")
endif()
