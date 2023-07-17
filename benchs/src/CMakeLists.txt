add_executable(CellntaBM
  ./SimpleWorldBM.cpp
  ./Main.cpp
)

target_link_libraries(CellntaBM PRIVATE
  Cellnta::Base
  Cellnta::World
  benchmark::benchmark
)
