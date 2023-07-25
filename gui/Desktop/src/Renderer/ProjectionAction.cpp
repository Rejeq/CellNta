#include "Renderer/ProjectionAction.h"

#include <Cellnta/Renderer/Renderer.h>

#include "Context.h"

using namespace Ui;
using namespace Ui::Action::Renderer;

static Cellnta::Renderer& GetRenderer(Context& ctx) {
  return ctx.GetRenderer();
}

void SetIgnoreCubeProject::Execute() {
  Cellnta::Renderer& ren = GetRenderer(*p_ctx);

  m_prevState = ren.GetIgnoreCubeProject();
  ren.SetIgnoreCubeProject(m_state);
}

void SetIgnoreCubeProject::Undo() {
  Cellnta::Renderer& ren = GetRenderer(*p_ctx);

  ren.SetIgnoreCubeProject(m_prevState);
}

void SetIgnoreCellProject::Execute() {
  Cellnta::Renderer& ren = GetRenderer(*p_ctx);

  m_prevState = ren.GetIgnoreCellProject();
  ren.SetIgnoreCellProject(m_state);
}

void SetIgnoreCellProject::Undo() {
  Cellnta::Renderer& ren = GetRenderer(*p_ctx);

  ren.SetIgnoreCellProject(m_prevState);
}
