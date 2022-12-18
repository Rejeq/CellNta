#pragma once

#include <Eigen/Core>
#include <spdlog/fmt/ostr.h>

template <typename T>
struct fmt::formatter<T,
  std::enable_if_t<
    std::is_base_of_v<Eigen::DenseBase<T>, T>, char>> :
      ostream_formatter {};
