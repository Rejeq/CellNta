set(TRACY_DIR ${CELLNTA_THIRD_PARTY}/tracy)

add_library(Tracy::Client INTERFACE IMPORTED)

target_sources(Tracy::Client INTERFACE ${TRACY_DIR}/TracyClient.cpp)
target_include_directories(Tracy::Client INTERFACE ${TRACY_DIR}/tracy)
target_link_libraries(Tracy::Client INTERFACE ${CMAKE_DL_LIBS})
target_compile_definitions(Tracy::Client INTERFACE TRACY_ENABLE)
