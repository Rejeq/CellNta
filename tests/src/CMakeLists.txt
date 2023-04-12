add_executable(CellntaBaseTests
  ./AreaTest.cpp
)

target_link_libraries(CellntaBaseTests PRIVATE
  Cellnta::Base
  GTest::gtest_main
)

gtest_discover_tests(CellntaBaseTests)
