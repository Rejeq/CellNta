#include "Cellnta/Algorithms/AlgoRandom.h"

#include "Cellnta/Config.h"

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

void AlgoRandom::Update() {
  CELLNTA_PROFILE;

  if (p_dim == 0)
    return;

  for (int i = 0; i < GetStep(); ++i) {
    Cell::Pos pos = Cell::Pos::NullaryExpr(
        p_dim, RandomRange<Cell::Point>(m_rangeMin, m_rangeMax, m_gen));
    m_data.push_back(pos);
  }
  p_needLoadInRenderer = true;
}

void AlgoRandom::LoadWorld(RenderData* data) {
  CELLNTA_PROFILE;

  if (data == nullptr)
    return;

  if (p_needLoadInRenderer) {
    for (auto& pos : m_data)
      data->SetCell(Cell(pos, 1));
    p_needLoadInRenderer = false;
    data->DesireAreaProcessed();
    return;
  }

  if (data->DesireArea()) {
    const std::vector<Area>& rects = data->GetDesireArea();

    for (auto& pos : m_data) {
      for (const auto& rect : rects) {
        if (rect.PosValid(pos)) {
          data->SetCell(Cell(pos, 1));
          break;
        }
      }
    }
    data->DesireAreaProcessed();
  }
}

void AlgoRandom::SetDimension(int dim) {
  CELLNTA_PROFILE;

  if (dim == p_dim)
    return;
  p_dim = dim;

  m_data.clear();
}

void AlgoRandom::SetRangeMin(int min) {
  if (min > m_rangeMax)
    return;
  m_rangeMin = min;
}

void AlgoRandom::SetRangeMax(int max) {
  if (max < m_rangeMin)
    return;
  m_rangeMax = max;
}
