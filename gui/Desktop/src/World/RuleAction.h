#pragma once

#include <Cellnta/Cell.h>
#include <Cellnta/World/Rule.h>

#include "ActionBase.h"

namespace Ui::Action::Rule {

class SetDimension : public Action::UndoBase {
 public:
  SetDimension(int dim) : m_dim(dim) {}

  void Execute() override;
  void Undo() override;

 private:
  // Change dimension can lost some data, so store whole rule
  std::unique_ptr<Cellnta::Rule> m_prevRule;
  int m_dim;
};

class SetTotalStates : public Action::UndoBase {
 public:
  SetTotalStates(size_t totalStates) : m_totalStates(totalStates) {}

  void Execute() override;
  void Undo() override;

 private:
  // Change total states can lost some data, so store whole rule
  std::unique_ptr<Cellnta::Rule> m_prevRule;
  size_t m_totalStates;
};

class SetState : public Action::UndoBase {
 public:
  SetState(Cellnta::Cell::State targetState, Cellnta::Rule::Mask mask,
           Cellnta::Cell::State newBecome)
      : m_mask(mask), m_targetState(targetState), m_become(newBecome) {}

  void Execute() override;
  void Undo() override;

 private:
  Cellnta::Rule::Mask m_mask;
  Cellnta::Cell::State m_targetState;
  Cellnta::Cell::State m_become;
  Cellnta::Cell::State m_prevBecome;
};

class ResetState : public Action::UndoBase {
 public:
  ResetState(Cellnta::Cell::State targetState, Cellnta::Rule::Mask mask)
      : m_mask(mask), m_targetState(targetState) {}

  void Execute() override;
  void Undo() override;

 private:
  Cellnta::Rule::Mask m_mask;
  Cellnta::Cell::State m_targetState;
  Cellnta::Cell::State m_prevBecome;
};

class ReplaceState : public Action::UndoBase {
 public:
  ReplaceState(Cellnta::Cell::State targetState, Cellnta::Rule::Mask oldMask,
               Cellnta::Rule::Mask newMask)
      : m_oldMask(oldMask), m_newMask(newMask), m_targetState(targetState) {}

  void Execute() override;
  void Undo() override;

 private:
  Cellnta::Rule::Mask m_oldMask;
  Cellnta::Rule::Mask m_newMask;
  Cellnta::Cell::State m_targetState;
};

class SetFallbackState : public Action::UndoBase {
 public:
  SetFallbackState(Cellnta::Cell::State targetState,
                   Cellnta::Cell::State fallback)
      : m_targetState(targetState), m_fallback(fallback) {}

  void Execute() override;
  void Undo() override;

 private:
  Cellnta::Cell::State m_targetState;
  Cellnta::Cell::State m_fallback;
  Cellnta::Cell::State m_prevFallback;
};

}  // namespace Ui::Action::Rule
