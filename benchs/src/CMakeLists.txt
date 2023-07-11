add_executable(CellntaBM
  ./Main.cpp
)

target_link_libraries(CellntaBM PRIVATE
  Cellnta::Base
  Cellnta::World
  benchmark::benchmark
)
