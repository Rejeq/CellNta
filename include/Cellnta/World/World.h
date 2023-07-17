#pragma once

#include <vector>

#include "Cellnta/Adjustable.h"
#include "Cellnta/Area.h"
#include "Cellnta/World/Rule.h"
#include "Cellnta/World/WorldIter.h"

namespace Cellnta {

enum class WorldType {
  RANDOM,
  SIMPLE,
  COUNT,
};

class World : public Adjustable {
 public:
  using AxisId = uint32_t;
  using AxisSize = int;
  using AxisSizeList = std::vector<AxisSize>;

  World(WorldType type) : m_type(type) {}
  virtual ~World() = default;

  virtual void Update() = 0;
  virtual size_t GetPopulation() const = 0;

  virtual bool SetRule(const Rule& rule) = 0;
  virtual const Rule& GetRule() const = 0;

  // TODO: If you wanna to add support for repeatable worlds, you can specify
  // that axis is repeatable by setting -1 in AxisSize
  virtual bool SetAxisSizeFor(AxisId axis, AxisSize size) = 0;
  virtual bool SetAxisSizeList(const AxisSizeList& axisList) = 0;
  virtual AxisSize GetAxisSizeFor(AxisId axis) const = 0;
  virtual AxisSizeList GetAxisSizeList() const = 0;

  virtual WorldIter MakeWholeIter() const = 0;
  virtual WorldIter MakeAreaIter(const Area& area) const = 0;

  int GetDimension() const { return GetRule().GetDimension(); }

  void SetStep(int step) { m_step = step; }
  int GetStep() const { return m_step; }

  WorldType GetType() const { return m_type; }

  void SetupFrom(const World& left) {
    m_step = left.m_step;
    SetRule(left.GetRule());
    SetAxisSizeList(left.GetAxisSizeList());
  }

 private:
  int m_step = 1;
  const WorldType m_type;
};

}  // namespace Cellnta
