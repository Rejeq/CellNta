find_package(GTest)

add_subdirectory(./utils/)

include(GoogleTest)
add_subdirectory(./src/)
add_subdirectory(./src/World/)
