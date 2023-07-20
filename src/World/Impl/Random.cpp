#include "Cellnta/World/Impl/Random.h"

#include "Cellnta/Config.h"
#include "Cellnta/Log.h"

using namespace Cellnta;

namespace {
template <typename OutScalar, typename Container>
struct RandomRange {
  RandomRange(std::default_random_engine& gen, const Container& min,
              const Container& max)
      : gen(gen), min(min), max(max) {}

  OutScalar operator()() const {
    auto res = min[i] + (gen() % (max[i] - min[i]));
    i++;
    return res;
  }

 private:
  std::default_random_engine& gen;
  const Container& min;
  const Container& max;
  mutable int i = 0;
};

}  // namespace

class WorldImplRandom::WholeIter : public IterBase::CellForward {
 public:
  WholeIter(const WorldImplRandom& world) : m_world(world) { Reset(); }

  void Reset() override { m_iter = m_world.m_data.begin(); }

  const Cell* Next() override {
    if (m_iter == m_world.m_data.end())
      return nullptr;

    m_curr.pos = *m_iter;
    m_curr.state = 1;

    ++m_iter;
    return &m_curr;
  }

 private:
  const WorldImplRandom& m_world;
  std::vector<Cell::Pos>::const_iterator m_iter;
  Cell m_curr;
};

class WorldImplRandom::AreaIter : public IterBase::CellForward {
 public:
  AreaIter(const WorldImplRandom& world, const Area& area)
      : m_world(world), m_area(area) {
    Reset();
  }

  void Reset() override { m_iter = m_world.m_data.begin(); }

  Cell* Next() override {
    while (true) {
      if (m_iter == m_world.m_data.end())
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
  const WorldImplRandom& m_world;
  std::vector<Cell::Pos>::const_iterator m_iter;
  Area m_area;
  Cell m_curr;
};

void WorldImplRandom::Update() {
  CELLNTA_PROFILE;

  for (int i = 0; i < GetStep(); ++i) {
    Cell::Pos pos = Cell::Pos::NullaryExpr(
        m_rule.GetDimension(),
        RandomRange<Cell::Pos::Scalar, AxisSizeList>(m_gen, m_min, m_max));
    m_data.push_back(pos);
  }
}

bool WorldImplRandom::SetRule(const Rule& rule) {
  CELLNTA_PROFILE;

  const int oldDim = m_rule.GetDimension();
  const int newDim = rule.GetDimension();
  if (oldDim != newDim && !m_max.empty()) {
    m_min = AxisSizeList(newDim, 0);
    m_max.resize(newDim, m_max.back());
    m_data.clear();
  }

  m_rule = rule;
  return false;
}

bool WorldImplRandom::SetAxisSizeFor(World::AxisId axis, World::AxisSize size) {
  if (axis <= 0 || axis >= (World::AxisId)m_rule.GetDimension())
    return true;
  if (size <= 0)
    return true;

  m_max[axis] = size;
  return false;
}

bool WorldImplRandom::SetAxisSizeList(const World::AxisSizeList& axisList) {
  if (axisList.size() > (World::AxisId)m_rule.GetDimension())
    return true;

  if (std::any_of(axisList.begin(), axisList.end(),
                  [](AxisSize size) { return size < 0; }))
    return true;

  m_min = AxisSizeList(axisList.size(), 0);
  m_max = axisList;
  return false;
}

World::AxisSize WorldImplRandom::GetAxisSizeFor(AxisId axis) const {
  assert(axis >= 0 && axis < (World::AxisId)m_rule.GetDimension());
  return m_max[axis];
}

World::AxisSizeList WorldImplRandom::GetAxisSizeList() const {
  return m_max;
}

bool WorldImplRandom::OnSetCell(const Cell& cell) {
  CELLNTA_PROFILE;

  m_data.push_back(cell.pos);
  return false;
}

Cell::State WorldImplRandom::OnGetCell(const Cell::Pos& pos) const {
  CELLNTA_PROFILE;

  if (pos.size() != m_rule.GetDimension())
    return (Cell::State)0;

  for (const auto& inPos : m_data) {
    if (inPos == pos)
      return (Cell::State)1;
  }

  return (Cell::State)0;
}

WorldIter WorldImplRandom::MakeWholeIter() const {
  return WorldIter::MakeImpl(WorldImplRandom::WholeIter(*this));
}

WorldIter WorldImplRandom::MakeAreaIter(const Area& area) const {
  return WorldIter::MakeImpl(WorldImplRandom::AreaIter(*this, area));
}

void WorldImplRandom::SetSeed(int seed) {
  m_seed = seed;
  m_gen = std::default_random_engine(seed);
}
