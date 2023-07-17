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

  bool SetAxisSizeFor(AxisId axis, AxisSize size) override;
  bool SetAxisSizeList(const AxisSizeList& axisList) override;
  AxisSize GetAxisSizeFor(AxisId axis) const override;
  AxisSizeList GetAxisSizeList() const override;

  WorldIter MakeWholeIter() const override;
  WorldIter MakeAreaIter(const Area& area) const override;

  void SetSeed(int seed);
  int GetSeed() { return m_seed; }

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos&) const override;

 private:
  class WholeIter;
  class AreaIter;

  std::vector<Cell::Pos> m_data;
  AxisSizeList m_min = {0, 0};
  AxisSizeList m_max;
  int m_seed = 123;
  std::default_random_engine m_gen{123};
};

}  // namespace Cellnta
