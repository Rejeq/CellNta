#include "Cellnta/World/Impl/Simple.h"

// TODO: Delete this
#include <chrono>

#include <Eigen/Core>

#include "Cellnta/Area.h"
#include "Cellnta/Axis.h"
#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

static void IteratorSetPosition(const size_t* stride, size_t idx,
                                Cell::Pos& pos) {
  CELLNTA_PROFILE;

  for (int i = pos.size() - 1; i >= 0; --i) {
    const uint32_t tmp = idx / stride[i];
    pos[i] = idx - stride[i] * tmp;
    idx = tmp;
  }
}

class WorldImplSimple::WholeIter : public IterBase::CellForward {
 public:
  WholeIter(const WorldImplSimple& world) : m_world(world) { Reset(); }

  void Reset() override {
    m_curr.pos = Cell::Pos::Zero(m_world.GetDimension());
    m_idx = 0;
  }

  const Cell* Next() override {
    CELLNTA_PROFILE;

    // TODO: Probaly previous algorithm must be more efficient when
    // active cells > disabled cells.
    // Because IteratorSetPosition() has always O(n) time and
    // IteratotNextPosition() has O(1) average and O(n) worst.
    // So maybe better to work with this algo when
    // population count is <= (GetTotalArea() / 2)
    // and use previous otherwise
    const Cell::State* data = m_world.GetWorld();

    for (; m_idx < m_world.GetTotalArea(); ++m_idx) {
      if (data[m_idx] != 0) {
        IteratorSetPosition(m_world.m_size.data(), m_idx, m_curr.pos);
        m_curr.state = data[m_idx];

        ++m_idx;  // Since would be infinity loop occured
        return &m_curr;
      }
    }
    return nullptr;
  }

 private:
  const WorldImplSimple& m_world;
  Cell m_curr;
  size_t m_idx = 0;
};

class WorldImplSimple::AreaIter : public IterBase::CellForward {
 public:
  AreaIter(const WorldImplSimple& world, const Area& area)
      : m_world(world), m_area(area) {
    Reset();
  }

  void Reset() override {
    if (!m_area.Valid()) {
      m_firstIter = -1;
      CELLNTA_LOG_ERROR("Unable to create AreaIterator: Area is invalid");
      return;
    }

    // TODO: Here a lot of checks, maybe decrease count?
    int totalDims = std::min(m_area.GetSize(), (int)m_world.m_size.size());
    for (int i = 0; i < totalDims; ++i) {
      auto& minPoint = m_area.Min()[i];
      auto& maxPoint = m_area.Max()[i];

      if (minPoint < 0)
        minPoint = 0;

      if (maxPoint <= 0) {
        m_firstIter = -1;
        CELLNTA_LOG_ERROR(
            "Unable to create AreaIterator: area.max has negative coordinates");
        return;
      }

      if (minPoint >= (int)m_world.m_size[i]) {
        m_firstIter = -1;
        CELLNTA_LOG_ERROR(
            "Unable to create AreaIterator: area.min has to large coordinates");
        return;
      }

      if (maxPoint > (int)m_world.m_size[i]) {
        maxPoint = m_world.m_size[i];
      }
    }

    m_curr.pos = Cell::Pos::Zero(m_world.GetDimension());
    m_idx = m_world.CalculateIdxFromPos(m_area.Min());
    // Since InrementPosition resets m_idx to intial state when it
    // reaches the end of
    m_idxEnd = m_idx;
    m_firstIter = true;

    if (m_idx >= m_world.GetTotalArea()) {
      CELLNTA_LOG_ERROR("Unable to reset area iterator: Something get wrong");
      m_firstIter = -1;
      return;
    }

    if (m_idx == 0) {
      // Its needed because IteratorNextPosition will be called before m_idx + 1
      m_curr.pos[m_curr.pos.size() - 1] = -1;
    } else {
      IteratorSetPosition(m_world.m_size.data(), m_idx - 1, m_curr.pos);
    }
  }

  const Cell* Next() override {
    CELLNTA_PROFILE;

    // Like in WorldImplSimple::Iterator it must be slower
    // when active cells < disabled cells.
    // But determining population in area will be much slower
    const Cell::State* data = m_world.GetWorld();

    if (m_firstIter == -1)
      return nullptr;

    while (true) {
      IncrementPosition();
      assert(m_area.PosValid(m_curr.pos) &&
             "m_curr.pos in AreaIterator must be always valid");

      if (m_idx == m_idxEnd && !m_firstIter)
        break;
      m_firstIter = false;

      if (data[m_idx] != 0) {
        m_curr.state = data[m_idx];
        return &m_curr;
      }
    }
    return nullptr;
  }

 private:
  void IncrementPosition() {
    CELLNTA_PROFILE;

    const size_t* stride = m_world.m_size.data();
    const Area& area = m_area;
    Cell::Pos& pos = m_curr.pos;
    size_t& idx = m_idx;

    size_t rowIdx = 1;
    for (int i = pos.size() - 1; i >= 0; --i) {
      const int min = std::max(area.MinAxis(i), 0);
      const int max = std::min(area.MaxAxis(i), (Cell::Pos::Scalar)stride[i]);

      assert(max >= min && "Max must >= min. Otherwise negative idx occured");

      if (!Axis::WithinLowerBound(min, pos[i])) {
        pos[i] = min;
        idx += rowIdx * (min - pos[i]);
        break;
      }

      pos[i] += 1;
      idx += rowIdx;
      if (Axis::WithinUpperBound(max, pos[i]))
        break;

      pos[i] = min;
      idx -= rowIdx * (max - min);

      rowIdx *= stride[i];
    }
  }

  const WorldImplSimple& m_world;
  Cell m_curr;
  Area m_area;
  size_t m_idx = 0;
  size_t m_idxEnd = 0;
  int m_firstIter = true;
};

void WorldImplSimple::Update() {
  CELLNTA_PROFILE;

  if (!IsWorldValid()) {
    CELLNTA_LOG_WARN(
        "WorldImplSimple not properly initiliezed. The Update() has not "
        "happened");
    return;
  }

  auto start = std::chrono::steady_clock::now();

  for (int i = 0; i < GetStep(); ++i) {
    const Cell::State* world = GetWorld();
    Cell::State* buffWorld = GetBufferWorld();
    m_population = 0;

    for (size_t currCell = 0; currCell < GetTotalArea(); ++currCell) {
      size_t neiCount = FindNeighbors(world, currCell);

      buffWorld[currCell] = m_bornMask[neiCount] || m_surviveMask[neiCount];
      m_population += m_bornMask[neiCount] || m_surviveMask[neiCount];
    }

    Step();
  }

  auto end = std::chrono::steady_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
  CELLNTA_LOG_INFO("Next generation time: {}", dur);
}

void WorldImplSimple::SetDimension(int dim) {
  CELLNTA_PROFILE;

  if (dim == p_dim)
    return;
  p_dim = dim;

  m_bornMask = boost::dynamic_bitset<>(std::pow(3, p_dim));
  m_surviveMask = boost::dynamic_bitset<>(std::pow(3, p_dim));

  // TODO: Remove this init
  if (p_dim > 2) {
    m_bornMask[4] = true;
    m_bornMask[5] = true;

    m_surviveMask[5] = true;
  }

  SetSize(std::vector<size_t>(p_dim, 30));
}

bool WorldImplSimple::SetAxisSizeFor(World::AxisId axis, World::AxisSize size) {
  if (axis < 0 || axis >= p_dim)
    return 1;
  if (size <= 0)
    return true;

  m_totalArea -= m_size[axis] + size;
  m_size[axis] = size;

  OnAxisSizeChanged();
  return false;
}

bool WorldImplSimple::SetAxisSizeList(const World::AxisSizeList& axisList) {
  if (axisList.size() != p_dim)
    return 1;

  size_t newArea = 1;
  for (auto i : axisList) {
    if (i <= 0)
      return true;
    newArea *= i;
  }

  m_totalArea = newArea;
  m_size.assign(axisList.begin(), axisList.end());

  OnAxisSizeChanged();
  return false;
}

bool WorldImplSimple::OnSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(cell.pos);

  if (world == nullptr || idx >= GetTotalArea())
    return true;

  const Cell::State state = world[idx];
  if (state == 0 && cell.state != 0)
    m_population += 1;
  else if (state != 0 && cell.state == 0)
    m_population -= 1;

  world[idx] = cell.state;
  return false;
}

Cell::State WorldImplSimple::OnGetCell(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  const Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(pos);

  if (world == nullptr || idx >= GetTotalArea())
    return 0;

  return world[idx];
}

WorldIter WorldImplSimple::MakeWholeIter() const {
  return WorldIter::MakeImpl(WorldImplSimple::WholeIter(*this));
}

WorldIter WorldImplSimple::MakeAreaIter(const Area& area) const {
  return WorldIter::MakeImpl(WorldImplSimple::AreaIter(*this, area));
}

void WorldImplSimple::SetSize(const std::vector<size_t>& size) {
  m_size = size;

  m_totalArea = 1;
  for (auto i : m_size)
    m_totalArea *= i;
  CreateWorld();
  GenerateNeigbors();
}

inline void WorldImplSimple::Step() {
  CELLNTA_PROFILE;

  m_oddGen = !m_oddGen;
}

size_t WorldImplSimple::FindNeighbors(const Cell::State* world,
                                      size_t idx) const {
  CELLNTA_PROFILE;

  size_t neiCount = 0;
  for (auto& i : m_neighbors) {
    size_t neiPos = FindIdxInRangedWorld(idx, i);

    if (neiPos < GetTotalArea())
      neiCount += (bool)world[neiPos];
  }
  return neiCount;
}

void WorldImplSimple::GenerateNeigbors() {
  CELLNTA_PROFILE;

  // Calculates cartesian product of {-1, 0, 1}, ...(p_dim times) sets
  // Without zero position

  constexpr std::array<int, 3> baseSet = {-1, 0, 1};
  const int totalSets = p_dim;

  const int totalSize = std::pow(baseSet.size(), totalSets);
  std::vector<int>& product = m_neighbors;
  product = std::vector<int>();
  product.reserve(totalSize);

  Cell::Pos tmpTuple = Cell::Pos(totalSets);
  for (int i = 0; i < totalSize; i++) {
    // Skip position where all values == 0
    if (i == totalSize / 2)
      continue;

    int limit = 1;  // Optimized std::pow()
    for (int j = 0; j < totalSets; j++) {
      const int idx = (i / limit) % baseSet.size();

      tmpTuple[j] = baseSet[idx];
      limit *= baseSet.size();
    }

    product.emplace_back(CalculateIdxFromPosRaw(tmpTuple));
  }
}

inline size_t WorldImplSimple::FindIdxInRangedWorld(size_t targetIdx,
                                                    size_t neighborIdx) const {
  CELLNTA_PROFILE;
  return targetIdx + neighborIdx;
}

size_t WorldImplSimple::CalculateIdxFromPos(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  if ((pos.size() - 2) > (int)m_size.size()) {
    CELLNTA_LOG_ERROR(
        "Unable to determine index from position: target pos > current dim");
    return GetTotalArea();
  }

  for (auto i = 0; i < pos.size(); ++i)
    if (pos[i] < 0 || pos[i] >= (int)m_size[i])
      return GetTotalArea();

  return CalculateIdxFromPosRaw(pos);
}

size_t WorldImplSimple::CalculateIdxFromPosRaw(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  size_t idx = 0;
  size_t size = 1;

  idx = pos[pos.size() - 1];
  for (int i = pos.size() - 2; i >= 0; --i) {
    size *= m_size[m_size.size() - 1 - i];
    idx += size * pos[i];
  }
  return idx;
}

void WorldImplSimple::OnAxisSizeChanged() {
  CreateWorld();
  GenerateNeigbors();
}

void WorldImplSimple::CreateWorld() {
  CELLNTA_PROFILE;

  m_population = 0;
  for (auto& world : m_worlds) {
    world = std::make_unique<Cell::State[]>(GetTotalArea());

    if (world == nullptr) {
      CELLNTA_LOG_ERROR("Unable to allocate memory for world in SimpleWorld");
      continue;
    }

    memset(world.get(), 0, GetTotalArea() * sizeof(Cell::State));
  }
}

void WorldImplSimple::DeleteWorld() {
  CELLNTA_PROFILE;

  m_population = 0;
  for (auto& world : m_worlds)
    world.reset(nullptr);
}
