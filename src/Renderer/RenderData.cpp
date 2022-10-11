#include "RenderData.h"

RenderData::RenderData()
{
  m_desiredArea.reserve(4);
}

RenderData::RenderData(size_t dim)
{
  m_d = dim;
  m_desiredArea.reserve(4);
}

void RenderData::SetChunk(const std::shared_ptr<Chunk>& chunk)
{
  m_data[chunk->GetPosition()] = chunk;
  m_needUpdate = true;
}

void RenderData::SetCell(const Eigen::VectorXi& pos, state_t state)
{
  ProfileScope;

  if (state == 0)
    return;

  if (m_visibleArea.CellValid(pos))
  {
    ChunkPos chunkPos = Chunk::GetPosFromGlobalPos(pos.cast<float>());
    auto res = m_data.find(chunkPos);
    if (res != m_data.end())
      res->second->SetCell(pos, state);
    else
    {
      std::shared_ptr<Chunk> chunk = std::make_shared<Chunk>(m_d, chunkPos.GetPosition());
      chunk->SetCell(pos, state);
      SetChunk(chunk);
    }
    m_needUpdate = true;
  }
}

const RenderData::Map& RenderData::GetData() const
{
  return m_data;
}

RenderData::ChunkPtr RenderData::GetChunk(const ChunkPos& pos)
{
  ProfileScope;

  auto res = m_data.find(pos);
  if (res != m_data.end())
    return res->second;
  return nullptr;
}

void RenderData::Clear()
{
  m_data.clear();
  m_needUpdate = true;
}

void RenderData::SetDistance(size_t distance)
{
  ProfileScope;

  if (distance == m_distance)
    return;

  m_distance = distance;
  m_visibleArea = GetDistanceArea() + m_pos.GetPosition();
  m_data.clear();
  m_desiredArea.emplace_back(m_visibleArea);
}

void RenderData::SetPosition(const ChunkPos& pos)
{
  ProfileScope;

  if (m_pos == pos)
    return;

  UpdateVisibleArea(pos.GetPosition());
  EraseUnvisibleArea(m_pos, pos);
  m_pos = pos;
}

void RenderData::UpdateVisibleArea(const Eigen::Vector3i& pos)
{
  ProfileScope;

  const Eigen::Vector3i cellPos = pos.array() * CHUNK_SIZE;
  const Area oldVisibleArea = m_visibleArea;
  m_visibleArea = GetDistanceArea() + cellPos;
  m_desiredArea = Area::InverseClip(m_visibleArea, oldVisibleArea);

  //for (size_t i = 0; i < m_desiredArea.size(); ++i)
  //  std::cout << i << ": "
  //  << "\t min: " << m_desiredArea[i].min.transpose()
  //  << "\t max: " << m_desiredArea[i].max.transpose()
  //  << std::endl;
  //std::cout << std::endl;
}

void RenderData::EraseUnvisibleArea(const ChunkPos& oldPos, const ChunkPos& newPos)
{
  ProfileScope;

  Area oldArea = GetDistanceChunkArea() + oldPos.GetPosition();
  Area newArea = GetDistanceChunkArea() + newPos.GetPosition();
  std::vector<Area> eraseList = Area::InverseClip(oldArea, newArea);

  Eigen::Vector3i pos;
  for (const Area& erase : eraseList)
  {
    for (pos.x() = erase.min.x(); pos.x() <= erase.max.x(); ++pos.x())
      for (pos.y() = erase.min.y(); pos.y() <= erase.max.y(); ++pos.y())
        for (pos.z() = erase.min.z(); pos.z() <= erase.max.z(); ++pos.z())
        {
          auto res = m_data.find(pos);
          if (res != m_data.end())
          {
            m_data.erase(res);
            m_needUpdate = true;
          }
        }
  }
  //for (size_t i = 0; i < eraseList.size(); ++i)
  //  std::cout << i << ": "
  //  << "\t min: " << eraseList[i].min.transpose()
  //  << "\t max: " << eraseList[i].max.transpose()
  //  << std::endl;
  //std::cout << std::endl;
}

bool RenderData::ChunkValid(const ChunkPos& pos)
{
  ProfileScope;

  Eigen::Vector3i maxPos = m_pos.GetPosition().array() + m_distance;
  Eigen::Vector3i minPos = m_pos.GetPosition().array() - m_distance;

  if ( (minPos.x() <= pos.x() && pos.x() <= maxPos.x())
    && (minPos.y() <= pos.y() && pos.y() <= maxPos.y())
    && (minPos.z() <= pos.z() && pos.z() <= maxPos.z())
    )
    return true;

  return false;
}

void Chunk::SetCell(const Eigen::VectorXi& pos, state_t state)
{
  ProfileScope;

  assert(pos.size() >= m_d);

  m_data[pos] = state;
  m_needUpdate = true;
}

void Chunk::Clear()
{
  m_data.clear();
}

Area::Area(const Eigen::Vector3i& min, const Eigen::Vector3i& max)
{
  this->min = min;
  this->max = max;
}

Area::Area(const int min, const int max)
{
  this->min.array() = min;
  this->max.array() = max;
}

bool Area::CellValid(const Eigen::VectorXi& cell) const
{
  ProfileScope;

  if (((min.x() <= cell.x()) && (cell.x() <= max.x()))
    && ((min.y() <= cell.y()) && (cell.y() <= max.y()))
    && ((min.z() <= cell.z()) && (cell.z() <= max.z()))
    )
    return true;
  return false;
}

std::vector<Area> Area::InverseClip(const Area& clipper, const Area& subject)
{
  ProfileScope;

  Area inter = Area::Intersection(clipper, subject);
  if (!inter.Valid())
    return { clipper };

  std::vector<Area> out;
  Area tmp;

  //Front area
  tmp = Area(Eigen::Vector3i(inter.max.x(), inter.min.y(), inter.min.z()), clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  //Behind area
  tmp = Area(clipper.min, Eigen::Vector3i(inter.min.x(), inter.max.y(), inter.max.z()));
  if (tmp.Valid())
    out.push_back(tmp);

  //Right area
  tmp = Area(Eigen::Vector3i(inter.min.x(), inter.min.y(), inter.max.z()), clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  //Left area
  tmp = Area(clipper.min, Eigen::Vector3i(inter.max.x(), inter.min.y(), inter.max.z()));
  if (tmp.Valid())
    out.push_back(tmp);

  //Top area
  tmp = Area(Eigen::Vector3i(inter.min.x(), inter.max.y(), inter.min.z()), clipper.max);
  if (tmp.Valid())
    out.push_back(tmp);

  //Bottom area
  tmp = Area(clipper.min, Eigen::Vector3i(inter.max.x(), inter.max.y(), inter.min.z()));
  if (tmp.Valid())
    out.push_back(tmp);

  return out;
}

Area Area::Intersection(const Area& first, const Area& second)
{
  ProfileScope;

  Area out;
  out.min.x() = std::max(first.min.x(), second.min.x());
  out.min.y() = std::max(first.min.y(), second.min.y());
  out.min.z() = std::max(first.min.z(), second.min.z());

  out.max.x() = std::min(first.max.x(), second.max.x());
  out.max.y() = std::min(first.max.y(), second.max.y());
  out.max.z() = std::min(first.max.z(), second.max.z());
  return out;
}

bool Area::Valid() const
{
  ProfileScope;

  if ( min.x() < max.x()
    && min.y() < max.y()
    && min.z() < max.z())
    return true;
  return false;
}
