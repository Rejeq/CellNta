#include "Cellnta/World/Impl/Random.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

namespace {
template <typename Scalar>
struct RandomRange {
  RandomRange(const Scalar& low, const Scalar& high,
              std::default_random_engine& gen)
      : dis(low, high), gen(gen) {}
  Scalar operator()() const { return dis(gen); }

 private:
  mutable std::uniform_int_distribution<> dis;
  std::default_random_engine& gen;
};

}  // namespace

class WorldImplRandom::Iterator : public Cellnta::Iterator {
 public:
  Iterator(const WorldImplRandom* world)
      : m_world(const_cast<WorldImplRandom*>(world)) {
    if (world == nullptr)
      CELLNTA_LOG_ERROR(
          "Passing a not initialized WorldImplRandom in Iterator");
    m_iter = m_world->m_data.begin();
  }

  const Cell* Next() override {
    if (m_iter == m_world->m_data.end())
      return nullptr;

    m_curr.pos = *m_iter;
    m_curr.state = 1;

    ++m_iter;
    return &m_curr;
  }

 private:
  WorldImplRandom* m_world;
  std::vector<Cell::Pos>::const_iterator m_iter;
  Cell m_curr;
};

class WorldImplRandom::AreaIterator : public Cellnta::Iterator {
 public:
  AreaIterator(const WorldImplRandom* world, const Area& area)
      : m_world(const_cast<WorldImplRandom*>(world)), m_area(area) {
    if (world == nullptr)
      CELLNTA_LOG_ERROR(
          "Passing a not initialized WorldImplRandom in AreaIterator");
    m_iter = m_world->m_data.begin();
  }

  Cell* Next() override {
    while (true) {
      if (m_iter == m_world->m_data.end())
        return nullptr;
      if (m_area.PosValid(*m_iter))
        break;
      ++m_iter;
    }

    m_curr.pos = *m_iter;
    m_curr.state = 1;

    ++m_iter;
    return &m_curr;
  }

 private:
  WorldImplRandom* m_world;
  std::vector<Cell::Pos>::iterator m_iter;
  Area m_area;
  Cell m_curr;
};

void WorldImplRandom::Update() {
  CELLNTA_PROFILE;

  if (p_dim == 0)
    return;

  for (int i = 0; i < GetStep(); ++i) {
    Cell::Pos pos = Cell::Pos::NullaryExpr(
        p_dim, RandomRange<Cell::Point>(m_rangeMin, m_rangeMax, m_gen));
    m_data.push_back(pos);
  }
}

void WorldImplRandom::SetDimension(int dim) {
  CELLNTA_PROFILE;

  if (dim == p_dim)
    return;
  p_dim = dim;

  m_data.clear();
}

bool WorldImplRandom::OnSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  m_data.push_back(cell.pos);
  return false;
}

Cell::State WorldImplRandom::OnGetCell(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  if (pos.size() != p_dim)
    return (Cell::State)0;

  for (const auto& inPos : m_data) {
    if (inPos == pos)
      return (Cell::State)1;
  }

  return (Cell::State)0;
}

std::unique_ptr<Cellnta::Iterator> WorldImplRandom::CreateIterator() const {
  return std::make_unique<WorldImplRandom::Iterator>(this);
}

std::unique_ptr<Cellnta::Iterator> WorldImplRandom::CreateIterator(
    const Area& area) const {
  return std::make_unique<WorldImplRandom::AreaIterator>(this, area);
}

void WorldImplRandom::SetRangeMin(int min) {
  if (min > m_rangeMax)
    return;
  m_rangeMin = min;
}

void WorldImplRandom::SetRangeMax(int max) {
  if (max < m_rangeMin)
    return;
  m_rangeMax = max;
}
