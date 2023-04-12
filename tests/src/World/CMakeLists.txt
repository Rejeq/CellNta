add_executable(CellntaWorldTests
  ./Main.cpp
  ./SimpleWorldTest.cpp
)

target_link_libraries(CellntaWorldTests PRIVATE
  Cellnta::World
  GTest::gtest_main
)

gtest_discover_tests(CellntaWorldTests)
