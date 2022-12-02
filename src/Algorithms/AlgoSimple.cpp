#include "Cellnta/Algorithms/AlgoSimple.h"

// TODO: Delete this
#include <chrono>

#include <Eigen/Core>

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

AlgoSimple::~AlgoSimple() { DeleteWorld(); }

void AlgoSimple::Update() {
  CELLNTA_PROFILE;

  auto start = std::chrono::steady_clock::now();

  if (p_dim == 0 || m_worlds[0] == nullptr || m_worlds[1] == nullptr)
    return;

  for (size_t i = 0; i < GetStep(); ++i) {
    const state_t* world = GetWorld();
    state_t* buffWorld = GetBufferWorld();

    for (size_t cellPos = 0; cellPos < GetTotalArea(); ++cellPos) {
      const size_t neiBitmask = FindNeighbors(world, cellPos);
      if (m_bornMask[neiBitmask] || m_surviveMask[neiBitmask])
        buffWorld[cellPos] = 1;
    }
    Step();
  }

  p_needLoadInRenderer = true;

  auto end = std::chrono::steady_clock::now();
  auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  CELLNTA_LOG_INFO("Next generation time: {}", dur);
}

void AlgoSimple::LoadWorld(RenderData* data) {
  CELLNTA_PROFILE;

  if (data == nullptr || GetWorld() == nullptr)
    return;

  state_t* world = GetWorld();
  Cell::Vec pos = Cell::Vec::Zero(GetDimensions());

  data->Clear();
  for (size_t cellPos = 0; cellPos < GetTotalArea(); ++cellPos) {
    if (world[cellPos] != 0)
      data->SetCell(pos, world[cellPos]);

    for (int i = pos.size() - 1; i >= 0; --i) {
      pos[i] += 1;
      if (pos[i] < m_size[i])
        break;
      pos[i] = 0;
      continue;
    }
  }
  p_needLoadInRenderer = false;
  data->DesireAreaProcessed();
}

void AlgoSimple::SetDimension(size_t dim) {
  CELLNTA_PROFILE;

  if (dim == p_dim)
    return;
  p_dim = dim;

  m_bornMask = boost::dynamic_bitset<>(std::pow(3, p_dim));
  m_surviveMask = boost::dynamic_bitset<>(std::pow(3, p_dim));

  m_bornMask[4] = 1;
  m_bornMask[5] = 1;

  m_surviveMask[5] = 1;

  SetSize(std::vector<size_t>(p_dim, 30));
  GenerateNeigbors({-1, 0, 1});

  // constexpr size_t posX = 27;
  // constexpr size_t posY = 15;
  // std::vector<Cell> glider2D(5, Cell(Eigen::VectorXi::Zero(3), 1));
  // glider2D[0].pos.block(0, 0, 3, 1) = Eigen::Vector3i(posX    , 15, posY );
  // glider2D[1].pos.block(0, 0, 3, 1) = Eigen::Vector3i(posX    , 15, posY +
  // 1); glider2D[2].pos.block(0, 0, 3, 1) = Eigen::Vector3i(posX + 1, 15, posY
  // + 2); glider2D[3].pos.block(0, 0, 3, 1) = Eigen::Vector3i(posX + 1, 15,
  // posY    ); glider2D[4].pos.block(0, 0, 3, 1) = Eigen::Vector3i(posX + 2,
  // 15, posY    ); SetCell(glider2D);

  // std::vector<Cell> stair(4, Cell(Eigen::VectorXi::Zero(dim), 1));
  // stair[0].pos.block(0, 0, 3, 1) = Eigen::Vector3i(2, 2, 3);
  // stair[1].pos.block(0, 0, 3, 1) = Eigen::Vector3i(2, 3, 3);
  // stair[2].pos.block(0, 0, 3, 1) = Eigen::Vector3i(2, 3, 2);

  // stair[3].pos.block(0, 0, 3, 1) = Eigen::Vector3i(3, 2, 2);

  // SetCell(stair);

  // constexpr int a = 15;
  // const int a = GetSize()[0] / 2;
  // std::vector<Cell> blinker(7, Cell(Eigen::VectorXi::Zero(p_dim), 1));
  // blinker[0].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a    , a    , a    );
  // blinker[1].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a - 1, a    , a    );
  // blinker[2].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a + 1, a    , a    );
  // blinker[3].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a    , a - 1, a    );
  // blinker[4].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a    , a + 1, a    );
  // blinker[5].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a    , a    , a - 1);
  // blinker[6].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a    , a    , a + 1);
  // SetCell(blinker);

  // constexpr int a = 15;
  // std::vector<Cell> line(3, Cell(Eigen::VectorXi::Zero(p_dim), 1));
  // line[0].pos.block(0, 0, 3, 1) = Eigen::Vector3i(5, 0, 0);
  // line[1].pos.block(0, 0, 3, 1) = Eigen::Vector3i(6, 0, 0);
  // line[2].pos.block(0, 0, 3, 1) = Eigen::Vector3i(7, 0, 0);
  // SetCell(line);

  // p_needLoadInRenderer = true;
}

void AlgoSimple::SetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  state_t* world = GetWorld();
  if (world == nullptr)
    return;

  size_t idx = CalculateIdxFromPos(cell.pos);
  if (idx >= GetTotalArea())
    return;
  world[idx] = cell.state;
  p_needLoadInRenderer = true;
}

void AlgoSimple::SetCell(const std::vector<Cell>& cells) {
  CELLNTA_PROFILE;

  state_t* world = GetWorld();
  if (world == nullptr)
    return;

  for (const auto& cell : cells) {
    size_t idx = CalculateIdxFromPos(cell.pos);
    if (idx >= GetTotalArea())
      continue;
    world[idx] = cell.state;
  }
  p_needLoadInRenderer = true;
}

state_t AlgoSimple::GetCell(const Cell::Vec& pos) {
  CELLNTA_PROFILE;

  state_t* world = GetWorld();
  if (world == nullptr)
    return 0;
  size_t idx = CalculateIdxFromPos(pos);
  if (idx >= GetTotalArea())
    return 0;
  return world[idx];
}

void AlgoSimple::SetSize(const std::vector<size_t>& size) {
  m_size = size;

  m_totalArea = 1;
  for (size_t i = 0; i < m_size.size(); ++i) m_totalArea *= m_size[i];
  CreateWorld();
}

void AlgoSimple::Step() {
  CELLNTA_PROFILE;

  m_oddGen = !m_oddGen;
  state_t* bufferWorld = GetBufferWorld();
  memset(bufferWorld, 0, GetTotalAreaInBytes());
}

size_t AlgoSimple::FindNeighbors(const state_t*& world, const size_t& idx) {
  CELLNTA_PROFILE;

  size_t out = 0;
  for (size_t i = 0; i < m_neighbors.size(); ++i) {
    size_t neiPos = FindIdxInRangedWorld(idx, m_neighbors[i]);

    if (neiPos < GetTotalArea())
      if (world[neiPos] != 0)
        ++out;
  }
  return out;
}

void AlgoSimple::GenerateNeigbors(const std::vector<cell_t>& oneDimNei) {
  CELLNTA_PROFILE;

  std::vector<std::vector<cell_t>> NDimNei(p_dim, oneDimNei);
  m_neighbors.clear();
  m_neighbors.reserve(std::pow(oneDimNei.size(), p_dim));

  // Cartesian product
  // https://stackoverflow.com/a/5279601
  using Vi = std::vector<cell_t>;
  std::vector<Vi::const_iterator> vd;
  vd.reserve(NDimNei.size());

  for (const auto& it : NDimNei) vd.push_back(it.begin());

  while (1) {
    Cell::Vec result = Cell::Vec::Zero(NDimNei.size());
    for (int i = vd.size() - 1; i >= 0; --i)
      result[result.size() - i - 1] = (*(vd[i]));
    m_neighbors.push_back(CalculateIdxFromPos(result));

    std::vector<Vi::const_iterator>::iterator it = vd.begin();
    int i = 0;
    while (1) {
      ++(*it);
      if (*it != NDimNei[i].end())
        break;
      if (it + 1 == vd.end())
        goto CartesianProductEnd;
      *it = NDimNei[i].begin();
      ++it;
      ++i;
    }
  }

CartesianProductEnd:

  // Erase position where all coordinates == 0
  int zeroPosIdx = m_neighbors.size() / 2;
  m_neighbors.erase(m_neighbors.begin() + zeroPosIdx);
}

size_t AlgoSimple::FindIdxInRangedWorld(const size_t& cellIdx,
                                        const size_t& neighborIdx) {
  CELLNTA_PROFILE;
  return cellIdx + neighborIdx;
}

void AlgoSimple::CalculatePositionFromIdx(size_t idx, Cell::Vec& out) {
  CELLNTA_PROFILE;

  for (int i = GetDimensions() - 1; i >= 0; --i) {
    const size_t size = m_size[i];
    uint32_t tmp = idx / size;
    out[i] = idx - size * tmp;
    idx = tmp;
  }
}

Cell::Vec AlgoSimple::CalculatePositionFromIdx(size_t idx) {
  CELLNTA_PROFILE;

  Cell::Vec out = Cell::Vec::Zero(GetDimensions());
  CalculatePositionFromIdx(idx, out);
  return out;
}

size_t AlgoSimple::CalculateIdxFromPos(const Cell::Vec& pos) {
  CELLNTA_PROFILE;

  size_t idx = 0;
  size_t size = 1;

  idx += pos[pos.size() - 1];
  for (int i = pos.size() - 2; i >= 0; --i) {
    size *= m_size[m_size.size() - 1 - i];
    idx += size * pos[i];
  }
  return idx;
}

void AlgoSimple::CreateWorld() {
  CELLNTA_PROFILE;

  for (size_t i = 0; i < 2; ++i) {
    if (m_worlds[i] != nullptr)
      delete[] m_worlds[i];
    m_worlds[i] = (state_t*)malloc(GetTotalAreaInBytes());
    if (m_worlds[i] == nullptr)
      throw "Unable to allocate memory";
    memset(m_worlds[i], 0, GetTotalAreaInBytes());
  }
}

void AlgoSimple::DeleteWorld() {
  CELLNTA_PROFILE;

  for (size_t i = 0; i < 2; ++i) {
    if (m_worlds[i] != nullptr) {
      delete[] m_worlds[i];
      m_worlds[i] = nullptr;
    }
  }
}
