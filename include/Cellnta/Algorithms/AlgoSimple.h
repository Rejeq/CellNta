#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "AlgoBase.h"

namespace Cellnta {

class AlgoSimple : public AlgoBase {
 public:
  AlgoSimple() : AlgoBase(AlgoType::SIMPLE) {}

  void Update() override;
  void LoadWorld(RenderData* data) override;
  void SetDimension(int dim) override;

  void SetCell(const Cell& cell) override;
  void SetCell(const std::vector<Cell>& cells) override;
  Cell::State GetCell(const Cell::Pos& pos) override;

  void SetCellRandomly();

  void SetSize(const std::vector<size_t>& size);
  void SetWorldRepeated(bool state) { m_worldRepeated = state; }
  void SetBorn(const boost::dynamic_bitset<>& bitmask) { m_bornMask = bitmask; }
  void SetSurvive(const boost::dynamic_bitset<>& bitmask) {
    m_surviveMask = bitmask;
  }

  const std::vector<size_t>& GetSize() const { return m_size; }
  bool GetWorldRepeated() const { return m_worldRepeated; }
  boost::dynamic_bitset<> GetBorn() const { return m_bornMask; }
  boost::dynamic_bitset<> GetSurvive() const { return m_surviveMask; }

 private:
  void Step();
  size_t FindNeighbors(const Cell::State* world, size_t idx);
  void GenerateNeigbors();

  inline size_t FindIdxInRangedWorld(size_t cellIdx, size_t neighborIdx);

  size_t CalculateIdxFromPos(const Cell::Pos& pos);

  void CreateWorld();
  void DeleteWorld();

  void CartesianProduct(int repeat, const Cell::Pos& oneDimNei,
                        std::vector<int>& out);

  Cell::State* GetWorld() { return m_worlds[m_oddGen].get(); }
  Cell::State* GetBufferWorld() { return m_worlds[!m_oddGen].get(); }

  inline size_t GetTotalArea() const { return m_totalArea; }
  inline size_t GetTotalAreaInBytes() const {
    return GetTotalArea() * sizeof(Cell::State);
  }

  std::array<std::unique_ptr<Cell::State[]>, 2> m_worlds;
  std::vector<size_t> m_size;
  size_t m_totalArea = 0;

  bool m_oddGen = false;
  bool m_worldRepeated = true;

  // If you want to activate at 0 neighbors, set only first bit
  boost::dynamic_bitset<> m_bornMask;
  boost::dynamic_bitset<> m_surviveMask;

  std::vector<int> m_neighbors;
};

}  // namespace Cellnta
