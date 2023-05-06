#pragma once

#include <climits>

namespace Cellnta {
namespace Axis {

constexpr bool WithinLowerBound(int min, int val) {
  return val >= min;
}

constexpr bool WithinUpperBound(int max, int val) {
  return val < max;
}

constexpr bool WithinBound(int min, int max, int val) {
  return WithinLowerBound(min, val) && WithinUpperBound(max, val);
}

// This function is useful when value is same as lower bound.
// It handle situations when WithinLowerBound() must return true.
//
// For example in std::map<T> exist method - lower_bound(key) which
// returns an iterator pointing to the first element that is *not less* than key
constexpr int NotLess(int val) {
  return val;
}

// This function is useful when value is same as upper bound.
// It handle situations when the WithinUpperBound() must return true.
//
// For example in std::map<T> exist a method - upper_bound(key) which
// returns an iterator pointing to the first element that is *greater* than key
constexpr int GreaterThan(int val) {
  return val - 1;
}

constexpr int GetInvalidLowerBound() {
  return INT_MIN;
}

constexpr int GetInvalidUpperBound() {
  return INT_MAX;
}

}  // namespace Axis
}  // namespace Cellnta
