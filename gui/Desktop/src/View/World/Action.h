#pragma once

#include <random>

#include <Cellnta/Area.h>
#include <Cellnta/World/WorldList.h>

#include <ActionBase.h>

namespace Ui {
namespace Action {
namespace World {

// FIXME: If NextGeneration is called, then undo list becomes incorrect
// TODO: Implement UndoBase, looks like its hard task because need provide a
// tonn of additional classes and algortihms
class NextGeneration : public Action::Base {
 public:
  void Execute() override;
};

class SetWorldType : public Action::UndoBase {
 public:
  SetWorldType(Cellnta::WorldType type) : m_type(type) {}
  void Execute() override;
  void Undo() override;

 private:
  Cellnta::WorldType m_type;
  Cellnta::WorldType m_prevType;
};

class SetDimension : public Action::UndoBase {
 public:
  SetDimension(int dim) : m_dim(dim) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_dim;
  int m_prevDim;
};

class SetStep : public Action::UndoBase {
 public:
  SetStep(int step) : m_step(step) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_step;
  int m_prevStep;
};

class SetCell : public Action::UndoBase {
 public:
  SetCell(const Cellnta::Cell& cell) : m_cell(cell) {}
  void Execute() override;
  void Undo() override;

 private:
  void Detail(const Cellnta::Cell& cell);

  Cellnta::Cell m_cell;
  Cellnta::Cell::State m_prevState;
  Action::BasePtr m_renAction;
};

}  // namespace World

namespace SimpleWorld {

class SetRepeated : public Action::UndoBase {
 public:
  SetRepeated(bool repeated) : m_repeated(repeated) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_repeated;
  int m_prevRepeated;
};

class SetSize : public Action::UndoBase {
 public:
  SetSize(const std::vector<size_t>& size) : m_size(size) {}
  void Execute() override;
  void Undo() override;

 private:
  std::vector<size_t> m_size;
  std::vector<size_t> m_prevSize;
};

class SetBorn : public Action::UndoBase {
 public:
  SetBorn(const boost::dynamic_bitset<>& born) : m_born(born) {}
  void Execute() override;
  void Undo() override;

 private:
  boost::dynamic_bitset<> m_born;
  boost::dynamic_bitset<> m_prevBorn;
};

class SetSurvive : public Action::UndoBase {
 public:
  SetSurvive(const boost::dynamic_bitset<>& survive) : m_survive(survive) {}
  void Execute() override;
  void Undo() override;

 private:
  boost::dynamic_bitset<> m_survive;
  boost::dynamic_bitset<> m_prevSurvive;
};

}  // namespace SimpleWorld

namespace RandomWorld {

class SetRangeMin : public Action::UndoBase {
 public:
  SetRangeMin(int min) : m_min(min) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_min;
  int m_prevMin;
};

class SetRangeMax : public Action::UndoBase {
 public:
  SetRangeMax(int max) : m_max(max) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_max;
  int m_prevMax;
};

class SetSeed : public Action::UndoBase {
 public:
  SetSeed(int seed) : m_seed(seed) {}
  void Execute() override;
  void Undo() override;

 private:
  int m_seed;
  int m_prevSeed;
};

}  // namespace RandomWorld

}  // namespace Action
}  // namespace Ui
