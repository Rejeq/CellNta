#pragma once

#include <Cellnta/Area.h>
#include <Cellnta/World/WorldList.h>

#include <ActionBase.h>

namespace Ui {
namespace Action {
namespace World {

class NextGeneration : public Action::Base {
 public:
  void Execute() override;
};

class SetWorldType : public Action::Base {
 public:
  SetWorldType(Cellnta::WorldType type) : m_type(type) {}
  void Execute() override;

 private:
  Cellnta::WorldType m_type;
};

class SetDimension : public Action::Base {
 public:
  SetDimension(int dim) : m_dim(dim) {}
  void Execute() override;

 private:
  int m_dim;
};

class SetStep : public Action::Base {
 public:
  SetStep(int step) : m_step(step) {}
  void Execute() override;

 private:
  int m_step;
};

class SetCell : public Action::Base {
 public:
  SetCell(const Cellnta::Cell& cell) : m_cell(cell) {}
  void Execute() override;

 private:
  Cellnta::Cell m_cell;
};

}  // namespace World

namespace SimpleWorld {

class SetRepeated : public Action::Base {
 public:
  SetRepeated(bool repeated) : m_repeated(repeated) {}
  void Execute() override;

 private:
  int m_repeated;
};

class SetSize : public Action::Base {
 public:
  SetSize(const std::vector<size_t>& size) : m_size(size) {}
  void Execute() override;

 private:
  std::vector<size_t> m_size;
};

class SetBorn : public Action::Base {
 public:
  SetBorn(const boost::dynamic_bitset<>& born) : m_born(born) {}
  void Execute() override;

 private:
  boost::dynamic_bitset<> m_born;
};

class SetSurvive : public Action::Base {
 public:
  SetSurvive(const boost::dynamic_bitset<>& survive) : m_survive(survive) {}
  void Execute() override;

 private:
  boost::dynamic_bitset<> m_survive;
};

}  // namespace SimpleWorld

namespace RandomWorld {

class SetRangeMin : public Action::Base {
 public:
  SetRangeMin(int min) : m_min(min) {}
  void Execute() override;

 private:
  int m_min;
};

class SetRangeMax : public Action::Base {
 public:
  SetRangeMax(int max) : m_max(max) {}
  void Execute() override;

 private:
  int m_max;
};

class SetSeed : public Action::Base {
 public:
  SetSeed(int seed) : m_seed(seed) {}
  void Execute() override;

 private:
  int m_seed;
};

}  // namespace RandomWorld

}  // namespace Action
}  // namespace Ui
