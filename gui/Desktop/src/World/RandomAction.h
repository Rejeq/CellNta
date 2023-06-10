#pragma once

#include <ActionBase.h>

namespace Ui::Action::RandomWorld {

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

}  // namespace Ui::Action::RandomWorld
