#include "Renderer/ShuffleAction.h"

#include <Cellnta/Renderer/NCellStorage.h>
#include <Cellnta/Renderer/RenderData.h>

#include "Context.h"

using namespace Ui;
using namespace Ui::Action::Shuffle;

Cellnta::ShuffleAxisStorage* GetShuffle(Context& ctx) {
  auto* data = ctx.GetRenderer().GetData();
  if (data == nullptr) {
    DESKTOP_LOG_WARN("Render data is not a valid");
    return nullptr;
  }

  return &data->GetCells();
}

void SetAxis::Execute() {
  auto* shuffle = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!shuffle, "Shuffle::SetAxis"))
    return;

  if ((m_axis < 0 || m_axis >= shuffle->GetDimension()) ||
      (m_with < 0 || m_with >= shuffle->GetDimension())) {
    DESKTOP_LOG_ERROR(DESKTOP_ACTION_DERR_MSG
                      "World::SetWorldType: Out of bound axis value");
    return;
  }

  m_prevWith = shuffle->GetShuffleAxis(m_axis);
  shuffle->SetShuffleAxis(m_axis, m_with);
}

void SetAxis::Undo() {
  auto* shuffle = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!shuffle, "Shuffle::SetAxis"))
    return;

  shuffle->SetShuffleAxis(m_axis, m_prevWith);
}

void SetShuffleOnDraw::Execute() {
  auto* cells = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!cells, "Shuffle::SetShuffleOnDraw"))
    return;

  m_prevState = cells->NeedShuffleOnDraw();
  cells->SetShuffleOnDraw(m_state);
}

void SetShuffleOnDraw::Undo() {
  auto* cells = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!cells, "Shuffle::SetShuffleOnDraw"))
    return;

  cells->SetShuffleOnDraw(m_prevState);
}

void Reset::Execute() {
  auto* shuffle = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!shuffle, "Shuffle::Reset"))
    return;

  int axisCount = shuffle->GetDimension();
  m_prev.reserve(axisCount);
  for (int i = 0; i < axisCount; i++)
    m_prev.emplace_back(shuffle->GetShuffleAxis(i));

  shuffle->ResetAxisShuffle();
}

void Reset::Undo() {
  auto* shuffle = GetShuffle(*p_ctx);
  if (Action::CheckLogDErr(!shuffle, "Shuffle::Reset"))
    return;

  for (size_t axis = 0; axis < m_prev.size(); axis++)
    shuffle->SetShuffleAxis(axis, m_prev[axis]);
}
