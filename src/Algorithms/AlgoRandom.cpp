#include "Cellnta/Algorithms/AlgoRandom.h"

#include "Cellnta/Config.h"

using namespace Cellnta;

namespace
{
  template<typename Scalar>
  struct RandomRange {
    RandomRange(const Scalar& low, const Scalar& high,
      std::default_random_engine& gen) : dis(low, high), gen(gen) {}
    const Scalar operator()() const { return dis(gen); }
    mutable std::uniform_int_distribution<> dis;
    std::default_random_engine& gen;
  };
}


void AlgoRandom::Update()
{
  CELLNTA_PROFILE;

  if (p_dim == 0)
    return;

  for (size_t i = 0; i < GetStep(); ++i)
  {
    Eigen::VectorXi pos = Eigen::VectorXi::NullaryExpr(p_dim, RandomRange<int>(m_rangeMin, m_rangeMax, m_gen));
    m_data.push_back(pos);
  }
  p_needLoadInRenderer = true;
}

void AlgoRandom::LoadWorld(RenderData* data)
{
  CELLNTA_PROFILE;

  if (data == nullptr)
    return;

  if (p_needLoadInRenderer)
  {
    for (size_t i = 0; i < m_data.size(); ++i)
      data->SetCell(m_data[i], 1);
    p_needLoadInRenderer = false;
    data->DesireAreaProcessed();
  }
  else if (data->DesireArea())
  {
    const std::vector<Area>& rects = data->GetDesireArea();

    for (size_t i = 0; i < m_data.size(); ++i)
    {
      for (auto& rect : rects)
      {
        if (rect.CellValid(m_data[i]))
        {
          data->SetCell(m_data[i], 1);
          break;
        }
      }
    }
    data->DesireAreaProcessed();
  }
}

void AlgoRandom::SetDimension(size_t dim)
{
  CELLNTA_PROFILE;

  if (dim == p_dim)
    return;
  p_dim = dim;

  m_data.clear();
}

void AlgoRandom::SetRangeMin(int min)
{
  if (min > m_rangeMax)
    return;
  m_rangeMin = min;
}

void AlgoRandom::SetRangeMax(int max)
{
  if (max < m_rangeMin)
    return;
  m_rangeMax = max;
}
