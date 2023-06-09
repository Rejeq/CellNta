find_package(Eigen3 3.4.0 CONFIG)

option(CELLNTA_THIRD_PARTY_Eigen3_BUILD "Enable Eigen building. When disabled just header only library will be added" OFF)

if (NOT Eigen3_FOUND AND CELLNTA_FETCHING)
  set(EIGEN_URL "https://gitlab.com/libeigen/eigen/-/archive/3.4.0/eigen-3.4.0.tar.gz")
  message(STATUS "Loading Eigen3 from ${EIGEN_URL}")

  include(FetchContent)
  FetchContent_Declare(eigen3
    URL ${EIGEN_URL}
  )

  if(CELLNTA_THIRD_PARTY_Eigen3_BUILD)
    FetchContent_MakeAvailable(eigen3)
  else()
    FetchContent_GetProperties(eigen3)
    if(NOT eigen3_POPULATED)
      FetchContent_Populate(eigen3)
      add_library(eigen INTERFACE IMPORTED)
      add_library(Eigen3::Eigen ALIAS eigen)
      target_include_directories(eigen INTERFACE "${eigen3_SOURCE_DIR}")
    endif()
  endif()

  message(STATUS "Eigen3 configured successfully")
endif()
