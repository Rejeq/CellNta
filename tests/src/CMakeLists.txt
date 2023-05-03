add_executable(CellntaBaseTests
  ./Main.cpp
  ./AdjustableTest.cpp
  ./AreaTest.cpp
  ./CellTest.cpp
  ./IteratorTest.cpp
  ./SnapshotTest.cpp
)

target_link_libraries(CellntaBaseTests PRIVATE
  Cellnta::Base
  GTest::gtest_main
  GTest::gmock_main
)

gtest_discover_tests(CellntaBaseTests)
