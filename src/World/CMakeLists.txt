find_package(Boost)

add_library(CellntaWorld STATIC
  ./Impl/Random.cpp
  ./Impl/Simple.cpp
)
add_library(Cellnta::World ALIAS CellntaWorld)

target_link_libraries(CellntaWorld PUBLIC
  Cellnta::Base
  Boost::boost
)
