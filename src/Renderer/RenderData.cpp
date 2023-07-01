#include "Cellnta/Renderer/RenderData.h"

#include <chrono>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/LogFormatEigen.h"
#include "Cellnta/World/World.h"

using namespace Cellnta;

void RenderData::Update(const World& world) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Updating RenderData");

  auto start = std::chrono::steady_clock::now();

  NCellStorage::VecList& rawCells = m_cells.GetRaw();
  Area area = GetVisibleArea();

  rawCells.EraseArea(area);

  auto iter = world.MakeAreaIter(GetVisibleArea());
  while (const Cell* cell = iter.Next()) {
    ForceSetCell(*cell);
  }

  auto end = std::chrono::steady_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  CELLNTA_LOG_INFO("RenderData update time: {}", dur);
}

void RenderData::SetDimension(int dim) {
  m_cells.SetDimension(dim);
  m_desiredArea.emplace_back(GetVisibleArea());
  m_d = dim;
}

void RenderData::SetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  CELLNTA_LOG_TRACE("Trying to add cell");

  if (cell.state == 0)
    return;

  if (GetVisibleArea().PosValid(cell.pos)) {
    ForceSetCell(cell);
  }
}

void RenderData::ForceSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  m_cells.Add(cell.pos.cast<NCellStorage::Point>());
  CELLNTA_LOG_TRACE("Added cell: ({}): {}", cell.pos.transpose(), cell.state);
}

void RenderData::Clear() {
  m_cells.Clear();
}

void RenderData::SetDistance(int distance) {
  CELLNTA_PROFILE;

  if (distance == m_distance)
    return;

  m_distance = distance;
  m_cells.Clear();
  EraseUnvisibleArea(m_pos);

  UpdateVisibleArea();
  m_desiredArea.emplace_back(GetVisibleArea());
}

void RenderData::SetPosition(Eigen::Vector3i pos) {
  CELLNTA_PROFILE;

  pos /= PositionDivider;
  pos *= PositionDivider;

  if (pos == m_pos)
    return;

  CELLNTA_LOG_TRACE("Updating position from ({}) to ({})", m_pos.transpose(),
                    pos.transpose());
  AddVisibleArea(pos);
  EraseUnvisibleArea(pos);
  m_pos = pos;
  UpdateVisibleArea();
}

void RenderData::AddVisibleArea(const Eigen::Vector3i& newPos) {
  CELLNTA_PROFILE;

  const Area oldArea = GetVisibleArea();
  const Area newArea = GetArea() + newPos;
  m_desiredArea = Area::InverseClip(newArea, oldArea);

  for (size_t i = 0; i < m_desiredArea.size(); ++i)
    CELLNTA_LOG_TRACE("Desired area {} min: ({}) max: ({})", i,
                      m_desiredArea[i].Min().transpose(),
                      m_desiredArea[i].Max().transpose());
}

void RenderData::EraseUnvisibleArea(const Eigen::Vector3i& newPos) {
  CELLNTA_PROFILE;

  const Area oldArea = GetVisibleArea();
  const Area newArea = GetArea() + newPos;
  std::vector<Area> eraseList = Area::InverseClip(oldArea, newArea);

  NCellStorage::VecList& rawCells = m_cells.GetRaw();
  Eigen::Vector3i pos;

  for (const auto& area : eraseList) {
    rawCells.EraseArea(area);

    CELLNTA_LOG_TRACE("Erase unvisible area from: ({}) to: ({})",
                      area.Min().transpose(),
                      area.Max().transpose());
  }
}
