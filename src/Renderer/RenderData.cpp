#include "Cellnta/Renderer/RenderData.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"
#include "Cellnta/LogFormatEigen.h"

using namespace Cellnta;

RenderData::RenderData() {
  m_desiredArea.reserve(4);
}

RenderData::RenderData(int dim) : m_d(dim) {
  m_desiredArea.reserve(4);
}

void RenderData::SetDimension(int dim) {
  m_cells.SetDimension(dim);
  m_desiredArea.emplace_back(GetVisibleArea());
  m_d = dim;
}

void RenderData::SetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  if (cell.state == 0)
    return;

  if (GetVisibleArea().PosValid(cell.pos)) {
    m_cells.Add(cell.pos.cast<NCellStorage::Point>());
    CELLNTA_LOG_TRACE("Added cell: ({}): {}", cell.pos.transpose(), cell.state);
  }
}

void RenderData::Clear() {
  m_cells.clear();
}

void RenderData::SetDistance(int distance) {
  CELLNTA_PROFILE;

  if (distance == m_distance)
    return;

  m_distance = distance;
  m_cells.clear();
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
                      m_desiredArea[i].min.transpose(),
                      m_desiredArea[i].max.transpose());
}

void RenderData::EraseUnvisibleArea(const Eigen::Vector3i& newPos) {
  CELLNTA_PROFILE;

  const Area oldArea = GetVisibleArea();
  const Area newArea = GetArea() + newPos;
  std::vector<Area> eraseList = Area::InverseClip(oldArea, newArea);

  NCellStorage::VecList& rawCells = m_cells.GetRaw();
  Eigen::Vector3i pos;

  auto PosValid = [&](auto& pos) -> bool {
    for (const Area& erase : eraseList)
      if (erase.PosValid(pos.template cast<Cell::Point>()))
        return true;
    return false;
  };

  auto rem = std::remove_if(rawCells.begin(), rawCells.end(), PosValid);
  rawCells.erase(rem, rawCells.end());

  for (size_t i = 0; i < eraseList.size(); ++i)
    CELLNTA_LOG_TRACE("Erase list {} min: ({}) max: ({})", i,
                      eraseList[i].min.transpose(),
                      eraseList[i].max.transpose());
}

void RenderData::SetCollatingX(int x) {
  m_collX = x;
  m_desiredArea.push_back(GetVisibleArea());
}

void RenderData::SetCollatingY(int y) {
  m_collY = y;
  m_desiredArea.push_back(GetVisibleArea());
}

void RenderData::SetCollatingZ(int z) {
  m_collZ = z;
  m_desiredArea.push_back(GetVisibleArea());
}
