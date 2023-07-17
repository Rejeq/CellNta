add_executable(CellntaWorldTests
  ./Main.cpp
  ./RuleTest.cpp
  ./SimpleWorldTest.cpp
)

target_link_libraries(CellntaWorldTests PRIVATE
  Cellnta::World
  Cellnta::Test::Utils
  GTest::gtest_main
)

gtest_discover_tests(CellntaWorldTests)
