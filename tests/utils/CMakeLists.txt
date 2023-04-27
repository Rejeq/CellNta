add_library(CellntaTestUtils STATIC
  ./Utils.cpp
)
add_library(Cellnta::Test::Utils ALIAS CellntaTestUtils)

target_include_directories(CellntaTestUtils PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(CellntaTestUtils PRIVATE
  Cellnta::Base
)
