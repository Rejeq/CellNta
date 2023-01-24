find_package(Eigen3 3.4.0 CONFIG)

if (NOT Eigen3_FOUND AND CELLNTA_FETCHING)
  set(EIGEN_URL "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz")
  message(STATUS "Loading Eigen3 from ${EIGEN_URL}")

  include(FetchContent)
  FetchContent_Declare(eigen3
    URL ${EIGEN_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
  )

  FetchContent_MakeAvailable(eigen3)

  message(STATUS "Eigen3 configured successfully")
endif()
