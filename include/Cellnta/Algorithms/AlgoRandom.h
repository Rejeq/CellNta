#pragma once

#include <Eigen/Core>
#include <random>
#include <vector>

#include "AlgoBase.h"

namespace Cellnta {

class AlgoRandom : public AlgoBase {
 public:
  AlgoRandom() : AlgoBase(AlgoType::RANDOM) {}
  virtual ~AlgoRandom() = default;

  void Update() override;
  void LoadWorld(RenderData* data) override;
  void SetDimension(int dim) override;

  void SetCell(const Cell&) override {}
  void SetCell(const std::vector<Cell>&) override {}
  state_t GetCell(const Eigen::VectorXi&) override { return 0; }

  void SetSeed(int seed) { m_gen = std::default_random_engine(seed); }

  void SetRangeMin(int min);
  void SetRangeMax(int max);
  int GetRangeMin() const { return m_rangeMin; }
  int GetRangeMax() const { return m_rangeMax; }

 private:
  std::vector<Eigen::VectorXi> m_data;
  int m_rangeMin = -64;
  int m_rangeMax = 64;
  std::default_random_engine m_gen{123};
};

}  // namespace Cellnta
