#pragma once

#include <memory>
#include <utility>

namespace Cellnta {

template <typename T>
using DrawerPtr = std::unique_ptr<T>;

template <typename T, typename... Args>
inline DrawerPtr<T> MakeDrawerInstance(Args&&... args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}

}  // namespace Cellnta
