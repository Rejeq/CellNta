#pragma once

#include <Eigen/Core>
#include <random>
#include <vector>

#include "Cellnta/World/World.h"

namespace Cellnta {

class WorldImplRandom : public World {
 public:
  WorldImplRandom() : World(WorldType::RANDOM) {}
  virtual ~WorldImplRandom() = default;

  void Update() override;
  void SetDimension(int dim) override;

  void SetCell(const Cell& cell) override;
  void SetCell(const std::vector<Cell>& cells) override;
  Cell::State GetCell(const Cell::Pos&) const override;

  std::unique_ptr<Iterator> CreateIterator() const override;
  std::unique_ptr<Iterator> CreateIterator(const Area& area) const override;

  void SetSeed(int seed) { m_gen = std::default_random_engine(seed); }

  void SetRangeMin(int min);
  void SetRangeMax(int max);
  int GetRangeMin() const { return m_rangeMin; }
  int GetRangeMax() const { return m_rangeMax; }

 private:
  class Iterator;
  class AreaIterator;

  std::vector<Cell::Pos> m_data;
  int m_rangeMin = -64;
  int m_rangeMax = 64;
  std::default_random_engine m_gen{123};
};

}  // namespace Cellnta
