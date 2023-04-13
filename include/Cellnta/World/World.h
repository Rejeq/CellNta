#pragma once

#include "Cellnta/Adjustable.h"
#include "Cellnta/Area.h"
#include "Cellnta/Iterator.h"

namespace Cellnta {

enum class WorldType {
  RANDOM,
  SIMPLE,
  COUNT,
};

class World : public Adjustable, public Iterable {
 public:
  World(WorldType type) : m_type(type) {}
  virtual ~World() = default;

  virtual void Update() = 0;
  virtual void SetDimension(int dim) = 0;
  virtual size_t GetPopulation() const = 0;

  int GetDimension() const { return p_dim; }

  void SetStep(int step) { m_step = step; }
  int GetStep() const { return m_step; }

  WorldType GetType() const { return m_type; }

  void SetupFrom(const World& left) {
    m_step = left.m_step;
    SetDimension(left.p_dim);
  }

 protected:
  int p_dim = 0;

 private:
  int m_step = 1;
  const WorldType m_type;
};

}  // namespace Cellnta
