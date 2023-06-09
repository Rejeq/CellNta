if (NOT TRACY_DIR AND CELLNTA_FETCHING)
  set(TRACY_URL "https://github.com/wolfpld/tracy/archive/refs/tags/v0.9.tar.gz")
  message(STATUS "Loading Tracy from ${IMGUI_URL}")

  include(FetchContent)
  FetchContent_Declare(tracy
    URL ${TRACY_URL}
  )
  FetchContent_MakeAvailable(tracy)
  set(TRACY_DIR ${tracy_SOURCE_DIR})
endif()

add_library(Tracy::Client INTERFACE IMPORTED)

target_sources(Tracy::Client INTERFACE ${TRACY_DIR}/public/TracyClient.cpp)
target_include_directories(Tracy::Client INTERFACE ${TRACY_DIR}/public/tracy)
target_link_libraries(Tracy::Client INTERFACE ${CMAKE_DL_LIBS})
target_compile_definitions(Tracy::Client INTERFACE TRACY_ENABLE)

message(STATUS "Tracy configured successfully")
