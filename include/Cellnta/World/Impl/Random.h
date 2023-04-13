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
  size_t GetPopulation() const override { return m_data.size(); }

  std::unique_ptr<Iterator> CreateIterator() const override;
  std::unique_ptr<Iterator> CreateIterator(const Area& area) const override;

  void SetSeed(int seed);
  int GetSeed() { return m_seed; }

  void SetRangeMin(int min);
  void SetRangeMax(int max);
  int GetRangeMin() const { return m_rangeMin; }
  int GetRangeMax() const { return m_rangeMax; }

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos&) const override;

 private:
  class Iterator;
  class AreaIterator;

  std::vector<Cell::Pos> m_data;
  int m_rangeMin = -64;
  int m_rangeMax = 64;
  int m_seed = 123;
  std::default_random_engine m_gen{123};
};

}  // namespace Cellnta
