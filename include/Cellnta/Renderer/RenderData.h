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
  RenderData();
  RenderData(int dim);

  void Update(const World& world);

  void SetDimension(int dim);

  void SetCell(const Cell& cell);
  void Clear();

  const NCellStorage& GetCells() const { return m_cells; }
  NCellStorage& GetCells() { return m_cells; }

  int GetCollatingX() const { return m_collX; }
  int GetCollatingY() const { return m_collY; }
  int GetCollatingZ() const { return m_collZ; }

  void SetCollatingX(int x);
  void SetCollatingY(int y);
  void SetCollatingZ(int z);

  void SetDistance(int distance);
  uint32_t GetDistance() const { return m_distance; }
  int GetSpace() const { return m_distance * m_distance * m_distance; }

  void SetPosition(Eigen::Vector3i pos);

  const std::vector<Area>& GetDesireArea() const { return m_desiredArea; }
  bool DesireArea() const { return !m_desiredArea.empty(); }
  void DesireAreaProcessed() { m_desiredArea.clear(); }

 private:
  void ForceSetCell(const Cell& cell);

  void AddVisibleArea(const Eigen::Vector3i& newPos);
  void EraseUnvisibleArea(const Eigen::Vector3i& newPos);

  void UpdateVisibleArea() { m_visibleArea = GetArea() + m_pos; }

  Area GetArea() const { return {-m_distance, m_distance}; }
  const Area& GetVisibleArea() const { return m_visibleArea; }

  NCellStorage m_cells;
  Eigen::Vector3i m_pos = Eigen::Vector3i(0, 0, 0); // Always mod of PositionDivider
  Area m_visibleArea = Area(0, 0);

  int m_distance = 0;
  int m_d = 0;

  std::vector<Area> m_desiredArea;

  int m_collX = 0;
  int m_collY = 1;
  int m_collZ = 2;

  constexpr static int PositionDivider = 16; // Visible area will be updated after the given length
};

}  // namespace Cellnta
