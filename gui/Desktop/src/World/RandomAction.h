#pragma once

#include <ActionBase.h>

namespace Ui::Action::RandomWorld {

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
