add_executable(CellntaBaseBM
  ./CellForestBM.cpp
  ./Main.cpp
)

target_link_libraries(CellntaBaseBM PRIVATE
  Cellnta::Base
  benchmark::benchmark
)
