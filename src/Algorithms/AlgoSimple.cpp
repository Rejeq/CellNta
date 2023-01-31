#include "Cellnta/Algorithms/AlgoSimple.h"

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

class AlgoSimple::Iterator : public Cellnta::Iterator {
 public:
  Iterator(const AlgoSimple* algo) : m_algo(algo) {
    if (algo == nullptr)
      CELLNTA_LOG_ERROR("Passing a not initialized AlgoSimple in Iterator");
    m_curr.pos = Cell::Pos::Zero(m_algo->GetDimensions());
  }

  const Cell* Next() override {
    Cell::State* world = m_algo->GetWorld();
    for (; m_idx < m_algo->GetTotalArea(); ++m_idx) {
      if (world[m_idx] != 0) {
        ++m_idx;
        IteratorNextPosition(m_algo->m_size.data(), m_curr.pos);
        break;
      }

      IteratorNextPosition(m_algo->m_size.data(), m_curr.pos);
    }
    return nullptr;
  }

 private:
  const AlgoSimple* m_algo;
  Cell m_curr;
  size_t m_idx = 0;
};

class AlgoSimple::AreaIterator : public Cellnta::Iterator {
 public:
  AreaIterator(const AlgoSimple* algo, const Area& area)
      : m_algo(algo), m_area(area) {
    if (algo == nullptr)
      CELLNTA_LOG_ERROR("Passing a not initialized AlgoSimple in AreaIterator");
    m_curr.pos = Cell::Pos::Zero(m_algo->GetDimensions());
  }

  const Cell* Next() override {
    Cell::State* world = m_algo->GetWorld();

    for (; m_idx < m_algo->GetTotalArea(); ++m_idx) {
      // FIXME: Looks like its not a optimal solution,
      // probably better is using generalized pitch
      if (m_area.PosValid(m_curr.pos)) {
        if (world[m_idx] != 0) {
          m_curr.state = world[m_idx];
          ++m_idx;
          IteratorNextPosition(m_algo->m_size.data(), m_curr.pos);
          return &m_curr;
        }
      }

      IteratorNextPosition(m_algo->m_size.data(), m_curr.pos);
    }
    return nullptr;
  }

  const AlgoSimple* m_algo;
  Cell m_curr;
  Area m_area;
  size_t m_idx = 0;
};

void AlgoSimple::Update() {
  CELLNTA_PROFILE;

  if (p_dim == 0 || m_worlds[0] == nullptr || m_worlds[1] == nullptr) {
    CELLNTA_LOG_WARN(
        "AlgoSimple not properly initiliezed. The Update() has not happened");
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

void AlgoSimple::SetDimension(int dim) {
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

  if (p_dim == 3) {
    const int a = GetSize()[0] / 2;
    std::vector<Cell> blinker(7, Cell(Eigen::VectorXi::Zero(p_dim), 1));
    blinker[0].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a, a, a);
    blinker[1].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a - 1, a, a);
    blinker[2].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a + 1, a, a);
    blinker[3].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a, a - 1, a);
    blinker[4].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a, a + 1, a);
    blinker[5].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a, a, a - 1);
    blinker[6].pos.block(0, 0, 3, 1) = Eigen::Vector3i(a, a, a + 1);
    SetCell(blinker);
  }

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

  Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(cell.pos);

  if (world == nullptr || idx >= GetTotalArea())
    return;

  world[idx] = cell.state;
}

void AlgoSimple::SetCell(const std::vector<Cell>& cells) {
  CELLNTA_PROFILE;

  Cell::State* world = GetWorld();
  if (world == nullptr)
    return;

  for (const auto& cell : cells) {
    size_t idx = CalculateIdxFromPos(cell.pos);
    if (idx < GetTotalArea())
      world[idx] = cell.state;
  }
  p_needLoadInRenderer = true;
}

Cell::State AlgoSimple::GetCell(const Cell::Pos& pos) {
  CELLNTA_PROFILE;

  Cell::State* world = GetWorld();
  size_t idx = CalculateIdxFromPos(pos);

  if (world == nullptr || idx >= GetTotalArea())
    return 0;

  return world[idx];
}

std::unique_ptr<Cellnta::Iterator> AlgoSimple::CreateIterator() const {
  return std::make_unique<Iterator>(this);
}

std::unique_ptr<Cellnta::Iterator> AlgoSimple::CreateIterator(
    const Area& area) const {
  return std::make_unique<AreaIterator>(this, area);
}

void AlgoSimple::SetSize(const std::vector<size_t>& size) {
  m_size = size;

  m_totalArea = 1;
  for (auto& i : m_size)
    m_totalArea *= i;
  CreateWorld();
}

void AlgoSimple::Step() {
  CELLNTA_PROFILE;

  m_oddGen = !m_oddGen;
}

size_t AlgoSimple::FindNeighbors(const Cell::State* world, size_t idx) {
  CELLNTA_PROFILE;

  size_t out = 0;
  for (auto& i : m_neighbors) {
    size_t neiPos = FindIdxInRangedWorld(idx, i);

    if (neiPos < GetTotalArea())
      out += (bool)world[neiPos];
  }
  return out;
}

void AlgoSimple::GenerateNeigbors() {
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

size_t AlgoSimple::FindIdxInRangedWorld(size_t cellIdx, size_t neighborIdx) {
  CELLNTA_PROFILE;
  return cellIdx + neighborIdx;
}

size_t AlgoSimple::CalculateIdxFromPos(const Cell::Pos& pos) {
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

  for (auto& world : m_worlds) {
    world = std::make_unique<Cell::State[]>(GetTotalArea());

    if (world == nullptr) {
      CELLNTA_LOG_ERROR("Unable to allocate memory for world in SimpleAlgo");
      continue;
    }

    memset(world.get(), 0, GetTotalAreaInBytes());
  }
}

void AlgoSimple::DeleteWorld() {
  CELLNTA_PROFILE;

  for (auto& world : m_worlds)
    world.reset(nullptr);
}

void AlgoSimple::CartesianProduct(int repeat, const Cell::Pos& oneDimNei,
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
