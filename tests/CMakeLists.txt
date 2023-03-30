file(GLOB_RECURSE TEST_SOURCE_LIST
  src/*.h
  src/*.cpp
)

find_package(GTest)

add_executable(CellNta_Tests ${TEST_SOURCE_LIST})
target_link_libraries(CellNta_Tests PRIVATE
  CellNta
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CellNta_Tests)
