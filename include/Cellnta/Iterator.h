#pragma once

#include <memory>

#include <Cellnta/Cell.h>

namespace Cellnta {
namespace IterBase {

template <typename Cell = Cellnta::Cell>
class CellForwardEx {
 public:
  virtual ~CellForwardEx() = default;

  virtual void Reset() = 0;
  virtual const Cell* Next() = 0;

  const Cell* Nth(int n) {
    const Cell* cell = nullptr;
    for (int i = 0; i < n; ++i) {
      cell = Next();
      if (cell == nullptr)
        break;
    }
    return cell;
  }
};

typedef CellForwardEx<> CellForward;

}  // namespace IterBase
}  // namespace Cellnta
