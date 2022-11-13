#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "AlgoBase.h"

namespace Cellnta {

class AlgoSimple : public AlgoBase
{
public:
  AlgoSimple() : AlgoBase(AlgoType::SIMPLE) {}
  ~AlgoSimple();

  void Update() override;
  void LoadWorld(RenderData* data) override;
  void SetDimension(size_t dim) override;

  void SetCell(const Cell& cell) override;
  void SetCell(const std::vector<Cell>& cells) override;
  state_t GetCell(const Cell::Vec& pos) override;

  void SetCellRandomly();

  void SetSize(const std::vector<size_t>& size);
  void SetWorldRepeated(bool state) { m_worldRepeated = state; }
  void SetBorn(const boost::dynamic_bitset<>& bitmask) { m_bornMask = bitmask; }
  void SetSurvive(const boost::dynamic_bitset<>& bitmask) { m_surviveMask = bitmask; }

  const std::vector<size_t>& GetSize() const { return m_size; }
  bool GetWorldRepeated() const { return m_worldRepeated; }
  boost::dynamic_bitset<> GetBorn() const { return m_bornMask; }
  boost::dynamic_bitset<> GetSurvive() const { return m_surviveMask; }

private:
  void Step();
  size_t FindNeighbors(const state_t*& world, const size_t& idx);
  void GenerateNeigbors(const std::vector<cell_t>& oneDimNei);

  inline size_t FindIdxInRangedWorld(const size_t& cellIdx, const size_t& neighborIdx);

  void CalculatePositionFromIdx(size_t idx, Cell::Vec& out);
  Cell::Vec CalculatePositionFromIdx(size_t idx);
  size_t CalculateIdxFromPos(const Cell::Vec& pos);

  void CreateWorld();
  void DeleteWorld();

  state_t* GetWorld() { return (m_oddGen) ? m_worlds[0] : m_worlds[1]; }
  state_t* GetBufferWorld() { return (m_oddGen) ? m_worlds[1] : m_worlds[0]; }

  inline size_t GetTotalArea() const { return m_totalArea; }
  inline size_t GetTotalAreaInBytes() const { return GetTotalArea() * sizeof(state_t); }


  state_t* m_worlds[2] = { nullptr, nullptr };
  std::vector<size_t> m_size;
  size_t m_totalArea = 0;

  bool m_oddGen = false;
  bool m_worldRepeated = true;

  //If you want to activate at 0 neighbors, set only first bit
  boost::dynamic_bitset<> m_bornMask;
  boost::dynamic_bitset<> m_surviveMask;

  std::vector<int> m_neighbors;
};

} //namespace Cellnta
