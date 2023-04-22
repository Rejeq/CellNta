#include "Cellnta/World/Impl/Simple.h"

// TODO: Delete this
#include <chrono>

#include <Eigen/Core>

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

class WorldImplSimple::WholeIterator : public Cellnta::Iterator {
 public:
  WholeIterator(const WorldImplSimple& world) : m_world(world) { Reset(); }

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

class WorldImplSimple::AreaIterator : public Cellnta::Iterator {
 public:
  AreaIterator(const WorldImplSimple& world, const Area& area)
      : m_world(world), m_area(area) {
    Reset();
  }

  void Reset() override {
    if (!m_area.Valid()) {
      m_firstIter = -1;
      CELLNTA_LOG_ERROR("Unable to create AreaIterator: Area is invalid");
      return;
    }

    m_curr.pos = Cell::Pos::Zero(m_world.GetDimension());
    m_idx = m_world.CalculateIdxFromPos(m_area.min);
    // Since InrementPosition resets m_idx to intial state when it
    // reaches the end of
    m_idxEnd = m_idx;
    m_firstIter = true;

    if (m_idx < m_world.GetTotalArea()) {
      IteratorSetPosition(m_world.m_size.data(), m_idx - 1, m_curr.pos);
      return;
    }

    m_idx = m_idxEnd = 0;
    // Its needed because IteratorNextPosition will be called before m_idx + 1
    m_curr.pos[m_curr.pos.size() - 1] = -1;
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
      IncrementPosition(m_world.m_size.data(), m_area, m_idx, m_curr.pos);
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
  static void IncrementPosition(const size_t* stride, const Area& area,
                                size_t& idx, Cell::Pos& pos) {
    CELLNTA_PROFILE;

    for (int i = pos.size() - 1; i >= 0; --i) {
      const int min = std::max(area.min[i], 0);
      const int max = std::min(area.max[i], (Cell::Pos::Scalar)stride[i]);
      const int rowIdx = std::pow(stride[i], pos.size() - 1 - i);

      assert(max > min && "Max must > min. Otherwise negative idx occured");

      if (pos[i] < min) {
        pos[i] = min;
        // Should modify idx in this case?
        idx += rowIdx * (min - pos[i]);
        break;
      }

      pos[i] += 1;
      idx += rowIdx;
      if (pos[i] < max)
        break;

      pos[i] = min;
      idx -= rowIdx * (max - min);
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

  if (p_dim == 0 || m_worlds[0] == nullptr || m_worlds[1] == nullptr) {
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

    for (size_t cellPos = 0; cellPos < GetTotalArea(); ++cellPos) {
      size_t neiMask = FindNeighbors(world, cellPos);
      buffWorld[cellPos] = m_bornMask[neiMask] || m_surviveMask[neiMask];
      m_population += m_bornMask[neiMask] || m_surviveMask[neiMask];
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

  m_bornMask[4] = true;
  m_bornMask[5] = true;

  m_surviveMask[5] = true;

  SetSize(std::vector<size_t>(p_dim, 30));
  GenerateNeigbors();
}

bool WorldImplSimple::OnSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(cell.pos);

  if (world == nullptr || idx >= GetTotalArea())
    return true;

  if (world[idx] == 0)
    m_population += 1;
  world[idx] = cell.state;
  return false;
}

Cell::State WorldImplSimple::OnGetCell(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(pos);

  if (world == nullptr || idx >= GetTotalArea())
    return 0;

  return world[idx];
}

std::unique_ptr<Cellnta::Iterator> WorldImplSimple::CreateIterator() const {
  return std::make_unique<WholeIterator>(*this);
}

std::unique_ptr<Cellnta::Iterator> WorldImplSimple::CreateIterator(
    const Area& area) const {
  return std::make_unique<AreaIterator>(*this, area);
}

void WorldImplSimple::SetSize(const std::vector<size_t>& size) {
  m_size = size;

  m_totalArea = 1;
  for (auto& i : m_size)
    m_totalArea *= i;
  CreateWorld();
}

void WorldImplSimple::Step() {
  CELLNTA_PROFILE;

  m_oddGen = !m_oddGen;
}

size_t WorldImplSimple::FindNeighbors(const Cell::State* world,
                                      size_t idx) const {
  CELLNTA_PROFILE;

  size_t out = 0;
  for (auto& i : m_neighbors) {
    size_t neiPos = FindIdxInRangedWorld(idx, i);

    if (neiPos < GetTotalArea())
      out += (bool)world[neiPos];
  }
  return out;
}

void WorldImplSimple::GenerateNeigbors() {
  CELLNTA_PROFILE;

  Cell::Pos oneDimNei = Cell::Pos(3);
  oneDimNei[0] = -1;
  oneDimNei[1] = 0;
  oneDimNei[2] = 1;
  m_neighbors.clear();

  CartesianProduct(p_dim, oneDimNei, m_neighbors);

  // Erase position where all coordinates == 0
  int zeroPosIdx = m_neighbors.size() / 2;
  m_neighbors.erase(m_neighbors.begin() + zeroPosIdx);
}

size_t WorldImplSimple::FindIdxInRangedWorld(size_t cellIdx,
                                             size_t neighborIdx) const {
  CELLNTA_PROFILE;
  return cellIdx + neighborIdx;
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

void WorldImplSimple::CreateWorld() {
  CELLNTA_PROFILE;

  m_population = 0;
  for (auto& world : m_worlds) {
    world = std::make_unique<Cell::State[]>(GetTotalArea());

    if (world == nullptr) {
      CELLNTA_LOG_ERROR("Unable to allocate memory for world in SimpleWorld");
      continue;
    }

    memset(world.get(), 0, GetTotalAreaInBytes());
  }
}

void WorldImplSimple::DeleteWorld() {
  CELLNTA_PROFILE;

  m_population = 0;
  for (auto& world : m_worlds)
    world.reset(nullptr);
}

void WorldImplSimple::CartesianProduct(int repeat, const Cell::Pos& oneDimNei,
                                       std::vector<int>& out) {
  CELLNTA_PROFILE;

  // https://stackoverflow.com/a/5279601
  using Vi = Cell::Pos;
  std::vector<Cell::Pos> NDimNei(repeat, oneDimNei);
  std::vector<Vi::const_iterator> vd;

  out.reserve(std::pow(oneDimNei.size(), repeat));
  vd.reserve(NDimNei.size());

  for (const auto& it : NDimNei)
    vd.push_back(it.begin());

  while (true) {
    Cell::Pos result = Cell::Pos::Zero(NDimNei.size());
    for (int i = vd.size() - 1; i >= 0; --i)
      result[result.size() - i - 1] = (*(vd[i]));
    out.push_back(CalculateIdxFromPosRaw(result));

    auto it = vd.begin();
    int i = 0;
    while (true) {
      ++(*it);
      if (*it != NDimNei[i].end())
        break;
      if (it + 1 == vd.end())
        return;
      *it = NDimNei[i].begin();
      ++it;
      ++i;
    }
  }
}
