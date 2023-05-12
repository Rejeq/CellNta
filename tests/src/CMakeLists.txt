add_executable(CellntaBaseTests
  ./AdjustableTest.cpp
  ./AreaTest.cpp
  ./CellTest.cpp
  ./CellForestTest.cpp
  ./IteratorTest.cpp
  ./Main.cpp
  ./SnapshotTest.cpp
)

target_link_libraries(CellntaBaseTests PRIVATE
  Cellnta::Base
  Cellnta::Test::Utils
  GTest::gtest_main
  GTest::gmock_main
)

gtest_discover_tests(CellntaBaseTests)
