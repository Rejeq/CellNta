#include "Cellnta/World/Impl/Simple.h"

// TODO: Delete this
#include <chrono>

#include <Eigen/Core>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

static void IteratorNextPosition(const size_t* sizes, Cell::Pos& pos) {
  if (sizes == nullptr)
    return;

  for (int i = pos.size() - 1; i >= 0; --i) {
    pos[i] += 1;
    if (pos[i] < (Cell::Pos::Scalar)sizes[i])
      break;
    pos[i] = 0;
  }
}

static void IteratorSetPosition(const size_t* sizes, size_t idx,
                                Cell::Pos& pos) {
  for (int i = pos.size() - 1; i >= 0; --i) {
    const size_t size = sizes[i];
    uint32_t tmp = idx / size;
    pos[i] = idx - size * tmp;
    idx = tmp;
  }
}

class WorldImplSimple::Iterator : public Cellnta::Iterator {
 public:
  Iterator(const WorldImplSimple& world) : m_world(world) { Reset(); }

  void Reset() override {
    m_curr.pos = Cell::Pos::Zero(m_world.GetDimension());
    m_idx = 0;
  }

  const Cell* Next() override {
    Cell::State* data = m_world.GetWorld();

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
    m_idx = m_world.CalculateIdxFromPos(m_area.min);
    m_curr.pos = Cell::Pos::Zero(m_world.GetDimension());
    IteratorSetPosition(m_world.m_size.data(), m_idx - 1, m_curr.pos);
  }

  const Cell* Next() override {
    Cell::State* data = m_world.GetWorld();

    for (; m_idx < m_world.GetTotalArea(); ++m_idx) {
      IteratorNextPosition(m_world.m_size.data(), m_curr.pos);

      // FIXME: Looks like its not a optimal solution,
      // probably better is using generalized pitch
      if (m_area.PosValid(m_curr.pos) && data[m_idx] != 0) {
        m_curr.state = data[m_idx];

        ++m_idx;  // Since would be infinity loop occured
        return &m_curr;
      }
    }
    return nullptr;
  }

  const WorldImplSimple& m_world;
  Cell m_curr;
  Area m_area;
  size_t m_idx = 0;
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

    for (size_t cellPos = 0; cellPos < GetTotalArea(); ++cellPos) {
      size_t neiMask = FindNeighbors(world, cellPos);
      buffWorld[cellPos] = m_bornMask[neiMask] || m_surviveMask[neiMask];
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
  return std::make_unique<Iterator>(*this);
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
    return 0;
  }

  size_t idx = 0;
  size_t size = 1;

  idx += pos[pos.size() - 1];
  for (int i = pos.size() - 2; i >= 0; --i) {
    size *= m_size[m_size.size() - 1 - i];
    idx += size * pos[i];
  }
  return idx;
}

void WorldImplSimple::CreateWorld() {
  CELLNTA_PROFILE;

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
    out.push_back(CalculateIdxFromPos(result));

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
