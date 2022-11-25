add_library(Eigen3::Eigen INTERFACE IMPORTED)

target_include_directories(Eigen3::Eigen INTERFACE
  ${CMAKE_CURRENT_LIST_DIR}/Eigen
)
