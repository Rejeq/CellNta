#pragma once

#include <AlgoBase.h>
#include "Window.h"

namespace Ui
{
  class AlgoWindow : public Window
  {
    static constexpr WindowProperties DefaultProperties = {
      .Name = "Algorithm",
    };

  public:
    AlgoWindow() : Window(DefaultProperties) {}

    void Draw() override;

  private:
    void DrawBaseAlgoInfo(Lf::AlgoBase* algo);

    Lf::Cell m_setCell;
  };
}
