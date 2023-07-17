#pragma once

#include <Cellnta/World/World.h>

#include "Widgets/Timer.h"
#include "WindowBase.h"

namespace Ui {

class WorldWindow : public Window {
  static constexpr WindowProperties DefaultProperties = {
      .Name = "World",
  };

 public:
  WorldWindow() : Window(DefaultProperties) {}

  void Draw() override;

 private:
  void DrawBaseWorldInfo(Cellnta::World*& world);

  void DrawRuleAllStatesSelector(const Cellnta::World& world);
  void DrawRuleStateDataSelector(const Cellnta::Rule& rule,
                                 Cellnta::Cell::State state);
  void DrawRuleNewSpecialStateSelector(const Cellnta::World& world);
  bool DrawRuleMaskSelector(const Cellnta::Rule& rule,
                            Cellnta::Rule::Mask& mask);

  Widget::Timer m_timer;
  Cellnta::Cell m_setCell;
  Cellnta::Rule::Mask m_newMask = 0;
  Cellnta::Cell::State m_newBecome = 0;
  Cellnta::Cell::State m_newState = 0;
};

}  // namespace Ui
