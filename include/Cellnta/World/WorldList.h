#pragma once

#include <memory>

#define CELLNTA_ALL_ALGOS 1

#define CELLNTA_ALGO_RANDOM (CELLNTA_ALL_ALGOS || 1)
#define CELLNTA_ALGO_SIMPLE (CELLNTA_ALL_ALGOS || 1)

#include "Cellnta/World/World.h"

#if CELLNTA_ALGO_RANDOM
#include "Cellnta/World/Impl/Random.h"
#endif

#if CELLNTA_ALGO_SIMPLE
#include "Cellnta/World/Impl/Simple.h"
#endif

namespace Cellnta {

inline std::unique_ptr<World> CreateWorldInstance(const WorldType worldType) {
  switch (worldType) {
#if CELLNTA_ALGO_RANDOM
    case WorldType::RANDOM: return std::make_unique<WorldImplRandom>(); break;
#endif
#if CELLNTA_ALGO_SIMPLE
    case WorldType::SIMPLE: return std::make_unique<WorldImplSimple>(); break;
#endif
    default: break;
  }
  return nullptr;
}

}  // namespace Cellnta
