#include "Renderer/HypercubeView.h"

#include "Renderer/HypercubeAction.h"

#include "Widgets/ComboEnum.h"
#include "Widgets/Utils.h"

using namespace Ui;

void HypercubeView::Draw(Context& ctx, Cellnta::HypercubeStorage& cube) {
  CELLNTA_PROFILE;

  uint32_t dim = cube.GetDimensions();

  if (Widget::Input("Cube dimensions", &dim, 1,
                    ImGuiInputTextFlags_CharsDecimal)) {
    ctx.PushAction<Action::Renderer::SetHypercubeDimension>(dim);
  }

  static const std::array<ComboData<Cellnta::CubeMode>, 3> CubeModeData = {
      ComboData(Cellnta::CubeMode::POINTS, "Points"),
      ComboData(Cellnta::CubeMode::WIREFRAME, "Wireframe"),
      ComboData(Cellnta::CubeMode::POLYGON, "Polygon"),
  };

  Cellnta::CubeMode res = Cellnta::CubeMode::NONE;
  if (Widget::ComboEnum("Cube mode", cube.GetMode(), CubeModeData, res)) {
    ctx.PushAction<Action::Renderer::SetHypercubeMode>(res);
  }
}
