#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "Cellnta/World/World.h"

namespace Cellnta {

class WorldImplSimple : public World {
 public:
  WorldImplSimple() : World(WorldType::SIMPLE) {}

  void Update() override;
  void SetDimension(int dim) override;
  size_t GetPopulation() const override { return m_population; }

  WorldIter MakeWholeIter() const override;
  WorldIter MakeAreaIter(const Area& area) const override;

  void SetSize(const std::vector<size_t>& size);
  const std::vector<size_t>& GetSize() const { return m_size; }

  void SetWorldRepeated(bool state) { m_worldRepeated = state; }
  bool GetWorldRepeated() const { return m_worldRepeated; }

  void SetBorn(const boost::dynamic_bitset<>& bitmask) { m_bornMask = bitmask; }
  boost::dynamic_bitset<> GetBorn() const { return m_bornMask; }

  void SetSurvive(const boost::dynamic_bitset<>& bitmask) {
    m_surviveMask = bitmask;
  }
  boost::dynamic_bitset<> GetSurvive() const { return m_surviveMask; }

 protected:
  bool OnSetCell(const Cell& cell) override;
  Cell::State OnGetCell(const Cell::Pos& pos) const override;

 private:
  class WholeIter;
  class AreaIter;

  void Step();
  size_t FindNeighbors(const Cell::State* world, size_t idx) const;
  void GenerateNeigbors();

  size_t FindIdxInRangedWorld(size_t targetIdx, size_t neighborIdx) const;

  // With position checking, returns GetTotalArea() on failure
  size_t CalculateIdxFromPos(const Cell::Pos& pos) const;

  // Without any checking, can be return garbage when pos is invalid
  size_t CalculateIdxFromPosRaw(const Cell::Pos& pos) const;

  void CreateWorld();
  void DeleteWorld();

  Cell::State* GetWorld() const { return m_worlds[m_oddGen].get(); }
  Cell::State* GetBufferWorld() const { return m_worlds[!m_oddGen].get(); }

  size_t GetTotalArea() const { return m_totalArea; }

  bool IsWorldValid() const {
    return (m_worlds[0] != nullptr && m_worlds[1] != nullptr) && p_dim != 0 &&
           m_totalArea != 0;
  }

  std::array<std::unique_ptr<Cell::State[]>, 2> m_worlds;
  std::vector<size_t> m_size;
  std::vector<int> m_neighbors;

  // If you want to activate at 0 neighbors, set only first bit
  boost::dynamic_bitset<> m_bornMask;
  boost::dynamic_bitset<> m_surviveMask;

  size_t m_totalArea = 0;
  size_t m_population = 0;

  bool m_oddGen = false;
  bool m_worldRepeated = true;
};

}  // namespace Cellnta
