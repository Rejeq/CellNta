#pragma once

#include <AlgoBase.h>
#include "Window.h"

namespace Ui
{
  class AlgoWindow : public Window
  {
  public:
    AlgoWindow() : Window("Algorithm") {}

    void Draw() override;

  private:
    void DrawBaseAlgoInfo(Lf::AlgoBase* algo);
  };
}
