#pragma once

#include <cstdint>
#include <vector>

#include <Cellnta/World/Impl/Simple.h>

#include "ActionBase.h"

namespace Ui::Action::SimpleWorld {

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

}  // namespace Ui::Action::SimpleWorld
