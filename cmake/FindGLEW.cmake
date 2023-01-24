# FIXME: Messy, perhaps there is a better way?
# The problem is that FindGLEW.cmake is located by default in .../cmake/Modules
# but we're already using a FindGLEW.cmake in the ${CELLNTA_DIR}/cmake
# otherwise recursion occurs

list(REMOVE_ITEM CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")
find_package(GLEW 2.0.0)
list(APPEND CMAKE_MODULE_PATH "${CELLNTA_DIR}/cmake")

if (NOT GLEW_FOUND AND CELLNTA_FETCHING)
    set(GLEW_URL "https://github.com/Perlmint/glew-cmake/archive/refs/tags/glew-cmake-2.2.0.tar.gz")
    message(STATUS "Loading GLEW-cmake from ${GLEW_URL}")

  include(FetchContent)
  FetchContent_Declare(glew
    URL ${GLEW_URL}
    DOWNLOAD_EXTRACT_TIMESTAMP ON
  )

  FetchContent_GetProperties(glew)
  if(NOT glew_POPULATED)
    FetchContent_Populate(glew)

    add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()

  add_library(GLEW::GLEW ALIAS libglew_shared)

  message(STATUS "GLEW-cmake configured successfully")
endif()
