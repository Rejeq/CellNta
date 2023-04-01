find_package(GTest)

add_executable(CellntaBaseTests
  src/AreaTest.cpp
)

target_link_libraries(CellntaBaseTests PRIVATE
  CellntaBase
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(CellntaBaseTests)
