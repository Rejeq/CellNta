#include "View/Pattern/Action.h"

#include <Cellnta/Config.h>
#include <Cellnta/World/World.h>
#include <Cellnta/LogFormatEigen.h>

#include "Context.h"
#include "Log.h"
#include "View/Pattern/Window.h"
#include "View/Renderer/Action.h"

using namespace Ui;
using namespace Ui::Action;
using namespace Ui::Action::Pattern;

#define CHECK_DIMENSION(_actionName, _excpetedDim, _actualDim) \
  {                                                            \
    if (m_pos.size() != 3)                                     \
      DESKTOP_LOG_WARN(                                        \
          "{} action excpect {}d position, but actual - "      \
          "{}. Other dimension will be ignored",               \
          _actionName, _excpetedDim, _actualDim);              \
  }                                                            \
  do {                                                         \
  } while (false)

#define UNABLE_ADD_SNAPSHOT_MSG(_actionName) \
  DESKTOP_ACTION_DERR_MSG _actionName ": Cannot add snapshot"

static bool AddSnapshot(Context* ctx, const Cellnta::Snapshot& snapshot) {
  CELLNTA_PROFILE;

  if (Action::CheckLogErr(!ctx, "Unable to determie the context"))
    return true;

  Cellnta::World* world = &ctx->GetWorld();
  if (Action::CheckLogErr(!world,
                          "Unable to add pattern to the world: World is "
                          "nullptr"))
    return true;

  if (Action::CheckLogErr(world->GetDimension() != snapshot.GetDimension(),
                          "World and snapshot dimension mismatch"))
    return true;

  world->SetCell(snapshot.CreateIterator());
  ctx->PushAction(Action::Make(Action::Renderer::Update()));
  return false;
}

void Pattern::Base::Undo() {
  // TODO: Maybe cause a bug if ctx will be not immediatly or unordered called
  // Action::Renderer::Update::Execute()
  AddSnapshot(p_ctx, m_prevSnap);
}

void Pattern::Base::MakeUndoAble(const Cellnta::Snapshot& snap) {
  std::unique_ptr<Cellnta::Iterator> it = snap.CreateIterator();
  Cellnta::World& world = p_ctx->GetWorld();
  const Cellnta::Cell* cell = nullptr;

  m_prevSnap.SetDimension(snap.GetDimension());
  while ((cell = it->Next()) != nullptr) {
    Cellnta::Cell::State worldState = world.GetCell(cell->pos);
    m_prevSnap.SetCell(cell->pos, worldState);
  }
}

void SetMiddlePosition::Execute() {
  CELLNTA_PROFILE;

  Window* win = p_ctx->GetWindowByName("Pattern");
  if (Action::CheckLogErr(!win, DESKTOP_ACTION_DERR_MSG
                          "Pattern::SetMiddlePosition: "
                          "Cannot find 'Pattern' window"))
    return;
  PatternWindow* patternWin = (PatternWindow*)win;

  // FIXME: Hard coded
  // TODO: Need provide GetRange() in base world class
  patternWin->SetPosition(Eigen::Vector3i(15, 15, 15));
}

void CreateBlinker::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("CreateBlinker", 3, m_pos.size());
  Cellnta::Snapshot blinker;
  blinker.SetDimension(3);
  // blinker.SetRule(); // TODO: Add rule setter

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  blinker.SetCell(Eigen::Vector3i(x, y, z), 1);
  blinker.SetCell(Eigen::Vector3i(x - 1, y, z), 1);
  blinker.SetCell(Eigen::Vector3i(x + 1, y, z), 1);
  blinker.SetCell(Eigen::Vector3i(x, y - 1, z), 1);
  blinker.SetCell(Eigen::Vector3i(x, y + 1, z), 1);
  blinker.SetCell(Eigen::Vector3i(x, y, z - 1), 1);
  blinker.SetCell(Eigen::Vector3i(x, y, z + 1), 1);

  MakeUndoAble(blinker);
  if (AddSnapshot(p_ctx, blinker))
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
  if (AddSnapshot(p_ctx, stair))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::CreateStair"));
}

void Create1dLine::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create1dLine", 3, m_pos.size());

  Cellnta::Snapshot line;
  line.SetDimension(3);
  // line.SetRule(); // TODO: Add rule setter

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  line.SetCell(Eigen::Vector3i(x, y, z), 1);
  line.SetCell(Eigen::Vector3i(x + 1, y, z), 1);
  line.SetCell(Eigen::Vector3i(x + 2, y, z), 1);

  MakeUndoAble(line);
  if (AddSnapshot(p_ctx, line))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::Create1dLine"));
}

void Create2dGlider::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create2dGlider", 3, m_pos.size());

  Cellnta::Snapshot glider;
  glider.SetDimension(3);  // TODO: Use 2d
  // glider.SetRule(); // TODO: Add rule setter

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  glider.SetCell(Eigen::Vector3i(x, y, z), 1);
  glider.SetCell(Eigen::Vector3i(x, y, z + 1), 1);
  glider.SetCell(Eigen::Vector3i(x + 1, y, z + 2), 1);
  glider.SetCell(Eigen::Vector3i(x + 1, y, z), 1);
  glider.SetCell(Eigen::Vector3i(x + 2, y, z), 1);

  MakeUndoAble(glider);
  if (AddSnapshot(p_ctx, glider))
    DESKTOP_LOG_ERROR(UNABLE_ADD_SNAPSHOT_MSG("Pattern::Create2dGlider"));
}

#undef UNABLE_ADD_SNAPSHOT_MSG
#undef CHECK_DIMENSION
