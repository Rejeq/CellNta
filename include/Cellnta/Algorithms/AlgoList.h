#pragma once

#include <memory>

#define CELLNTA_ALL_ALGOS 1

#define CELLNTA_ALGO_RANDOM (CELLNTA_ALL_ALGOS || 1)
#define CELLNTA_ALGO_SIMPLE (CELLNTA_ALL_ALGOS || 1)

#include "AlgoBase.h"

#if CELLNTA_ALGO_RANDOM
#include "AlgoRandom.h"
#endif

#if CELLNTA_ALGO_SIMPLE
#include "AlgoSimple.h"
#endif

namespace Cellnta {

inline std::unique_ptr<AlgoBase> CreateAlgoInstance(const AlgoType algoType) {
  switch (algoType) {
#if CELLNTA_ALGO_RANDOM
    case AlgoType::RANDOM: return std::make_unique<AlgoRandom>(); break;
#endif
#if CELLNTA_ALGO_SIMPLE
    case AlgoType::SIMPLE: return std::make_unique<AlgoSimple>(); break;
#endif
    default: break;
  }
  return nullptr;
}

}  // namespace Cellnta
