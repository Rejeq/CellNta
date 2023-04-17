add_executable(CellntaBaseTests
  ./Main.cpp
  ./AdjustableTest.cpp
  ./AreaTest.cpp
  ./IteratorTest.cpp
)

target_link_libraries(CellntaBaseTests PRIVATE
  Cellnta::Base
  GTest::gtest_main
)

gtest_discover_tests(CellntaBaseTests)
