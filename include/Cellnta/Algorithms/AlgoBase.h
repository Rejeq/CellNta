#pragma once

//TODO: Avoid using this
#include "Cellnta/Renderer/RenderData.h"

namespace Cellnta {

enum class AlgoType
{
  RANDOM,
  SIMPLE,
  COUNT,
};

class AlgoBase
{
public:
  AlgoBase(AlgoType type) : m_type(type) {}
  virtual ~AlgoBase() = default;

  virtual void Update() = 0;
  virtual void LoadWorld(RenderData* data) = 0;
  virtual void SetDimension(size_t dim) = 0;

  virtual void SetCell(const Cell& cell) = 0;
  virtual void SetCell(const std::vector<Cell>& cells) = 0;
  virtual state_t GetCell(const Cell::Vec& pos) = 0;

	size_t GetDimensions() { return p_dim; }

  void SetStep(size_t step) { m_step = step; }
  size_t GetStep() const { return m_step; }

  AlgoType GetType() const { return m_type; }
	bool NeedLoadWorld() const { return p_needLoadInRenderer; }

  void SetupFrom(const AlgoBase& left)
  {
    m_step = left.m_step;
    SetDimension(left.p_dim);
  }

protected:
  bool p_needLoadInRenderer = false;
	size_t p_dim = 0;

private:
  size_t m_step = 1;
  const AlgoType m_type;
};

} // namespace Cellnta
