#pragma once

#include <memory>

#include <Cellnta/Cell.h>

namespace Cellnta {
namespace IterBase {

class CellForward {
 public:
  virtual ~CellForward() = default;

  virtual void Reset() = 0;
  virtual const Cell* Next() = 0;

  const Cell* Nth(int n);
};

}  // namespac: IterBase
}  // namespace Cellnta
