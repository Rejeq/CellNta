add_library(CellntaWorld STATIC
  ./Impl/Random.cpp
  ./Impl/Simple.cpp

  ./Rule.cpp
  ./WorldIter.cpp
)
add_library(Cellnta::World ALIAS CellntaWorld)

target_link_libraries(CellntaWorld PUBLIC
  Cellnta::Base
)
