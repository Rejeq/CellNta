#include "Pattern/Action.h"

#include <Cellnta/Config.h>
#include <Cellnta/LogFormatEigen.h>
#include <Cellnta/World/World.h>

#include "Context.h"
#include "Log.h"
#include "Renderer/RenderDataAction.h"

using namespace Ui;
using namespace Ui::Action;
using namespace Ui::Action::Pattern;

#define CHECK_DIMENSION(_actionName, _expectedDim, _actualDim) \
  {                                                            \
    if (_expectedDim != _actualDim)                            \
      DESKTOP_LOG_WARN(                                        \
          "{} action expect {}d position, but actual - "       \
          "{}. Other dimension will be ignored",               \
          _actionName, _expectedDim, _actualDim);              \
  }                                                            \
  do {                                                         \
  } while (false)

#define UNABLE_ADD_SNAPSHOT_MSG(_actionName) \
  DESKTOP_ACTION_DERR_MSG _actionName ": Cannot add snapshot"

static bool AddSnapshot(Context& ctx, const Cellnta::Snapshot& snapshot) {
  CELLNTA_PROFILE;

  Cellnta::World& world = ctx.GetWorld();

  if (Action::CheckLogErr(world.GetDimension() != snapshot.GetDimension(),
                          "World and snapshot dimension mismatch"))
    return true;

  // TODO: Add method that truncate or adds dimension of current pattern to
  // the specified dimension
  // world.SetCell(snapshot.MakeResizedWholeIter(world.GetDimension()));

  world.SetCell(snapshot.MakeWholeIter());
  ctx.PushAction<Action::Renderer::Update>();
  return false;
}

void Pattern::Base::Undo() {
  // TODO: Maybe cause a bug if ctx will be not immediatly or unordered called
  // Action::Renderer::Update::Execute()
  AddSnapshot(*p_ctx, m_prevSnap);
}

void Pattern::Base::MakeUndoAble(const Cellnta::Snapshot& snap) {
  CELLNTA_PROFILE;

  auto it = snap.MakeWholeIter();
  Cellnta::World& world = p_ctx->GetWorld();
  const Cellnta::Cell* cell = nullptr;

  m_prevSnap.SetDimension(snap.GetDimension());
  while ((cell = it.Next()) != nullptr) {
    Cellnta::Cell::State worldState = world.GetCell(cell->pos);
    m_prevSnap.SetCell(cell->pos, worldState);
  }
}

void SetMiddlePosition::Execute() {
  CELLNTA_PROFILE;

  Cellnta::World& world = p_ctx->GetWorld();

  assert(world.GetDimension() == m_pos.size() && "Dimensions must be a same");
  for (int i = 0; i < m_pos.size(); ++i) {
    m_pos[i] = world.GetAxisSizeFor(i) / 2;
  }
}

void CreateBlinker::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("CreateBlinker", 3, m_pos.size());

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  Cellnta::Snapshot blinker = {
      {Eigen::Vector3i(x, y, z), 1},

      {Eigen::Vector3i(x - 1, y, z), 1}, {Eigen::Vector3i(x + 1, y, z), 1},
      {Eigen::Vector3i(x, y - 1, z), 1}, {Eigen::Vector3i(x, y + 1, z), 1},
      {Eigen::Vector3i(x, y, z - 1), 1}, {Eigen::Vector3i(x, y, z + 1), 1},
  };

  // TODO: Maybe add a warning message about rule mismatch?
  // or force rule of current pattern to the world

  MakeUndoAble(blinker);
  if (AddSnapshot(*p_ctx, blinker))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::CreateBlinker"));
}

void CreateStair::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("CreateStair", 3, m_pos.size());

  Cellnta::Snapshot stair;
  stair.SetDimension(3);
  // stair.SetRule(); // TODO: Add rule setter

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  stair.SetCell(Eigen::Vector3i(x + 1, y, z), 1);
  stair.SetCell(Eigen::Vector3i(x, y + 1, z + 1), 1);
  stair.SetCell(Eigen::Vector3i(x, y + 1, z), 1);
  stair.SetCell(Eigen::Vector3i(x, y, z + 1), 1);

  MakeUndoAble(stair);
  if (AddSnapshot(*p_ctx, stair))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::CreateStair"));
}

void Create1dLine::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create1dLine", 1, m_pos.size());

  int x = m_pos.x();

  // 111
  Cellnta::Snapshot line = {
      {Eigen::Vector<int, 1>(x - 1), 1},
      {Eigen::Vector<int, 1>(x + 0), 1},
      {Eigen::Vector<int, 1>(x + 1), 1},
  };

  MakeUndoAble(line);
  if (AddSnapshot(*p_ctx, line))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::Create1dLine"));
}

void Create2dGlider::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create2dGlider", 2, m_pos.size());

  int x = m_pos.x();
  int y = m_pos.y();

  // .1.
  // ..1
  // 111
  Cellnta::Snapshot glider = {
      {Eigen::Vector2i(x + 0, y - 1), 1}, {Eigen::Vector2i(x + 1, y + 0), 1},

      {Eigen::Vector2i(x - 1, y + 1), 1}, {Eigen::Vector2i(x + 0, y + 1), 1},
      {Eigen::Vector2i(x + 1, y + 1), 1},
  };

  MakeUndoAble(glider);
  if (AddSnapshot(*p_ctx, glider))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::Create2dGlider"));
}

void Create2dDiod::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create2dDiod", 2, m_pos.size());

  int x = m_pos.x();
  int y = m_pos.y();

  // ..33...
  // 133.333
  // ..33...
  Cellnta::Snapshot diod = {
      {Eigen::Vector2i(x - 3, y + 0), 1}, {Eigen::Vector2i(x - 2, y + 0), 3},
      {Eigen::Vector2i(x - 1, y + 0), 3},

      {Eigen::Vector2i(x + 1, y + 0), 3}, {Eigen::Vector2i(x + 2, y + 0), 3},
      {Eigen::Vector2i(x + 3, y + 0), 3},

      {Eigen::Vector2i(x - 1, y - 1), 3}, {Eigen::Vector2i(x - 1, y + 1), 3},

      {Eigen::Vector2i(x + 0, y - 1), 3}, {Eigen::Vector2i(x + 0, y + 1), 3},
  };

  MakeUndoAble(diod);
  if (AddSnapshot(*p_ctx, diod))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::Create2dDiod"));
}

#undef UNABLE_ADD_SNAPSHOT_MSG
#undef CHECK_DIMENSION
