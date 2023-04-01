find_package(Boost)

add_library(CellntaWorld STATIC
  ./Impl/Random.cpp
  ./Impl/Simple.cpp
)

target_link_libraries(CellntaWorld PUBLIC
  CellntaBase
  Boost::boost
)
