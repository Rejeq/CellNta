#pragma once

#include <Cellnta/Algorithms/AlgoBase.h>
#include "Window.h"

namespace Ui {

class AlgoWindow : public Window
{
  static constexpr WindowProperties DefaultProperties = {
    .Name = "Algorithm",
  };

public:
  AlgoWindow() : Window(DefaultProperties) {}

  void Draw() override;

private:
  void DrawBaseAlgoInfo(Cellnta::AlgoBase* algo);

  Cellnta::Cell m_setCell;
};

} //namespace Ui
