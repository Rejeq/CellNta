#include "Cellnta/Iterator.h"

using namespace Cellnta;
using namespace Cellnta::IterBase;

const Cell* CellForward::Nth(int n) {
  const Cell* cell = nullptr;
  for (int i = 0; i < n; ++i) {
    cell = Next();
    if (cell == nullptr)
      break;
  }
  return cell;
}
