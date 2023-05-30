add_executable(CellntaRendererTests
  ./Main.cpp
  ./HypercubeStorageTest.cpp
)

target_link_libraries(CellntaRendererTests PRIVATE
  Cellnta::Renderer
  GTest::gtest_main
)

gtest_discover_tests(CellntaRendererTests)
