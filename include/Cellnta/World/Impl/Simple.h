#pragma once

#include <vector>

#include "Cellnta/World/World.h"

namespace Cellnta {

class WorldImplSimple : public World {
 public:
  WorldImplSimple() : World(WorldType::SIMPLE) {}

  void Update() override;
  size_t GetPopulation() const override { return m_population; }

  bool SetRule(const Rule& rule) override;
  const Rule& GetRule() const override { return m_rule; }

  bool SetAxisSizeFor(AxisId axis, AxisSize size) override;
  bool SetAxisSizeList(const AxisSizeList& axisList) override;
  AxisSize GetAxisSizeFor(AxisId axis) const override { return m_size[axis]; }
  AxisSizeList GetAxisSizeList() const override {
    return {m_size.begin(), m_size.end()};
  }

  WorldIter MakeWholeIter() const override;
  WorldIter MakeAreaIter(const Area& area) const override;

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos& pos) const override;

 private:
  class WholeIter;
  class AreaIter;

  void Step();
  Rule::Mask FindNeighbors(const Cell::State* world, size_t idx) const;
  void GenerateNeigbors();

  size_t FindIdxInRangedWorld(size_t targetIdx, size_t neighborIdx) const;

  // With position checking, returns GetTotalArea() on failure
  size_t CalculateIdxFromPos(const Cell::Pos& pos) const;

  // Without any checking, can be return garbage when pos is invalid
  size_t CalculateIdxFromPosRaw(const Cell::Pos& pos) const;

  void OnAxisSizeChanged();

  void CreateWorld();
  void DeleteWorld();

  Cell::State* GetWorld() const { return m_worlds[m_oddGen].get(); }
  Cell::State* GetBufferWorld() const { return m_worlds[!m_oddGen].get(); }

  size_t GetTotalArea() const { return m_totalArea; }

  bool IsWorldValid() const {
    return (m_worlds[0] != nullptr && m_worlds[1] != nullptr) && IsSizeValid();
  }

  bool IsSizeValid() const { return !m_size.empty() && m_totalArea != 0; }

  std::array<std::unique_ptr<Cell::State[]>, 2> m_worlds;
  std::vector<size_t> m_size;
  std::vector<int> m_neighbors;

  Rule m_rule = Rule(2, 2, Rule::Neighborhood::Moore);

  size_t m_totalArea = 0;
  size_t m_population = 0;

  bool m_oddGen = false;
};

}  // namespace Cellnta
