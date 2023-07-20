#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <Eigen/Core>

#include <Cellnta/Area.h>
#include <Cellnta/Renderer/NCellStorage.h>

namespace Cellnta {

class World;

class RenderData {
 public:
  RenderData() = default;
  RenderData(int dim) : m_d(dim) {}

  void UpdateArea(const World& world, const Area& area);
  void UpdateDesired(const World& world);
  void UpdateVisible(const World& world);

  void SetCell(const Cell& cell);
  void Clear();

  const NCellStorage& GetCells() const { return m_cells; }
  NCellStorage& GetCells() { return m_cells; }

  void SetDistance(int distance);
  uint32_t GetDistance() const { return m_distance; }
  int GetSpace() const { return m_distance * m_distance * m_distance; }

  void SetPosition(Eigen::Vector3i pos);

  void SetDimension(int dim);
  int GetDimension() const { return m_d; }

  const std::vector<Area>& GetDesireArea() const { return m_desiredArea; }
  bool DesireArea() const { return !m_desiredArea.empty(); }
  void DesireAreaProcessed() { m_desiredArea.clear(); }

 private:
  void ForceSetCell(const Cell& cell);

  void AddVisibleArea(const Eigen::Vector3i& newPos);
  void EraseInvisibleArea(const Eigen::Vector3i& newPos);

  void SyncVisibleAreaWithPos() { m_visibleArea = GetArea() + m_pos; }

  Area GetArea() const { return {-m_distance, m_distance}; }
  const Area& GetVisibleArea() const { return m_visibleArea; }

  NCellStorage m_cells;
  // Always mod of PositionDivider
  Eigen::Vector3i m_pos = Eigen::Vector3i(0, 0, 0);
  Area m_visibleArea = Area(0, 0);

  int m_distance = 0;
  int m_d = 0;

  std::vector<Area> m_desiredArea;

  // m_pos will be updated every time when integer part of the real position
  // changes after division by a number
  constexpr static int PositionDivider = 16;
};

}  // namespace Cellnta
