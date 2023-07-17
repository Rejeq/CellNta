#pragma once

#include <Cellnta/Cell.h>
#include <Cellnta/World/WorldList.h>

#include <ActionBase.h>

namespace Ui::Action::World {

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

class SetSize : public Action::UndoBase {
  using World = Cellnta::World;

 public:
  SetSize(World::AxisSizeList sizeList) : m_sizeList(std::move(sizeList)) {}

  void Execute() override;
  void Undo() override;

 private:
  World::AxisSizeList m_sizeList;
  World::AxisSizeList m_prevSizeList;
};

class SetAxisSize : public Action::UndoBase {
  using World = Cellnta::World;

 public:
  SetAxisSize(World::AxisId axis, World::AxisSize size)
      : m_axis(axis), m_size(size) {}

  void Execute() override;
  void Undo() override;

 private:
  World::AxisId m_axis;
  World::AxisSize m_size;
  World::AxisSize m_prevSize = -1;
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

}  // namespace Ui::Action::World
