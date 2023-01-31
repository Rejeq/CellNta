#pragma once

#include <Cellnta/Area.h>
#include <Cellnta/Iterator.h>

namespace Cellnta {

enum class AlgoType {
  RANDOM,
  SIMPLE,
  COUNT,
};

class AlgoBase : public Iterable {
 public:
  AlgoBase(AlgoType type) : m_type(type) {}
  virtual ~AlgoBase() = default;

  virtual void Update() = 0;
  virtual void SetDimension(int dim) = 0;

  virtual void SetCell(const Cell& cell) = 0;
  virtual void SetCell(const std::vector<Cell>& cells) = 0;
  virtual Cell::State GetCell(const Cell::Pos& pos) = 0;

  int GetDimensions() const { return p_dim; }

  void SetStep(int step) { m_step = step; }
  int GetStep() const { return m_step; }

  AlgoType GetType() const { return m_type; }

  void SetupFrom(const AlgoBase* left) {
    if (left == nullptr)
      return;

    m_step = left->m_step;
    SetDimension(left->p_dim);
  }

 protected:
  int p_dim = 0;

 private:
  int m_step = 1;
  const AlgoType m_type;
};

}  // namespace Cellnta
