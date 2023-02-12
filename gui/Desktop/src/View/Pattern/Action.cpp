#include "View/Pattern/Action.h"

#include <Cellnta/Config.h>
#include <Cellnta/Snapshot.h>
#include <Cellnta/World/World.h>

#include "Log.h"
#include "View/Pattern/Window.h"
#include "View/Renderer/Action.h"

using namespace Ui;
using namespace Ui::Action::Pattern;

#define CHECK_DIMENSION(_ActionName, _actualDim, _excpetedDim) \
  {                                                            \
    if (m_pos.size() != 3)                                     \
      DESKTOP_LOG_WARN(                                        \
          "{} action excpect {}d position, but actual - {}. "  \
          "Other dimension will be ignored",                   \
          _ActionName, _excpetedDim, _actualDim);              \
  }                                                            \
  do {                                                         \
  } while (false)

void SetMiddlePosition::Execute() {
  CELLNTA_PROFILE;

  Window* win = p_ctx->GetWindowByName("Pattern");
  if (win == nullptr) {
    DESKTOP_LOG_ERROR(
        "Unable to execute SetMiddlePosition action, because cannot find "
        "'Pattern' window");
    return;
  }
  PatternWindow* patternWin = (PatternWindow*)win;

  // FIXME: Hard coded
  // TODO: Need provide GetRange() in base world class
  patternWin->SetPosition(Eigen::Vector3i(15, 15, 15));
}

static bool AddSnapshot(Context* ctx, const Cellnta::Snapshot& snapshot) {
  CELLNTA_PROFILE;

  if (ctx == nullptr) {
    DESKTOP_LOG_ERROR("Unable to determie the context");
    return true;
  }

  Cellnta::World* world = &ctx->GetWorld();
  if (world == nullptr) {
    DESKTOP_LOG_WARN(
        "Unable to add pattern to the world, because world is nullptr");
    return true;
  }

  if (world->GetDimension() != snapshot.GetDimension()) {
    DESKTOP_LOG_ERROR("World and snapshot dimension mismatch");
    return true;
  }

  world->SetCell(snapshot.CreateIterator());
  ctx->PushAction(Action::Make(Action::Renderer::Update()));
  return false;
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

  if (AddSnapshot(p_ctx, blinker))
    DESKTOP_LOG_ERROR("Unable to add snapshot in CreateBlinker action");
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

  if (AddSnapshot(p_ctx, stair))
    DESKTOP_LOG_ERROR("Unable to add snapshot in CreateStair action");
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

  if (AddSnapshot(p_ctx, line))
    DESKTOP_LOG_ERROR("Unable to add snapshot in CreateStair action");
}

void Create2dGlider::Execute() {
  CELLNTA_PROFILE;

  CHECK_DIMENSION("Create2dGlider", 3, m_pos.size());

  Cellnta::Snapshot glider;
  glider.SetDimension(3);
  // glider.SetRule(); // TODO: Add rule setter

  int x = m_pos.x();
  int y = m_pos.y();
  int z = m_pos.z();

  glider.SetCell(Eigen::Vector3i(x, y, z), 1);
  glider.SetCell(Eigen::Vector3i(x, y, z + 1), 1);
  glider.SetCell(Eigen::Vector3i(x + 1, y, z + 2), 1);
  glider.SetCell(Eigen::Vector3i(x + 1, y, z), 1);
  glider.SetCell(Eigen::Vector3i(x + 2, y, z), 1);

  if (AddSnapshot(p_ctx, glider))
    DESKTOP_LOG_ERROR("Unable to add snapshot in CreateStair action");
}
