#pragma once

#include <memory>

#include <Cellnta/Area.h>

namespace Cellnta {

class Iterator {
 public:
  virtual ~Iterator() = default;

  virtual void Reset() = 0;
  virtual const Cell* Next() = 0;

  const Cell* Nth(int n);
};

using IteratorRef = std::unique_ptr<Iterator>;

class Iterable {
 public:
  virtual ~Iterable() = default;

  virtual IteratorRef CreateIterator() const = 0;
  virtual IteratorRef CreateIterator(const Area& area) const = 0;
};

}  // namespace Cellnta
