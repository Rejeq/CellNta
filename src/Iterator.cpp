#include "Cellnta/Iterator.h"

using namespace Cellnta;

const Cell* Iterator::Nth(int n) {
  const Cell* cell = nullptr;
  for (int i = 0; i < n; ++i)
    cell = Next();
  return cell;
}
