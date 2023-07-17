#pragma once

#include <Cellnta/World/World.h>

#include "WindowBase.h"

namespace Ui {

class Timer {
 public:
  void Update(float delta) {
    m_remaining -= delta;
    if (m_remaining < 0.0f) {
      if (m_res)
        m_res();
      m_remaining = m_init;
    }
  }

  void Start(float init, std::function<void()> res) {
    m_init = init;
    m_remaining = init;
    m_res = res;
  }

  void Stop() { m_remaining = FLT_MAX; }
  bool Enabled() const { return m_remaining != FLT_MAX; }

 private:
  float m_init = 0;
  float m_remaining = FLT_MAX;
  std::function<void()> m_res;
};

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

  Cellnta::Cell m_setCell;
  Timer m_timer;
  Cellnta::Rule::Mask m_newMask = 0;
  Cellnta::Cell::State m_newBecome = 0;
  Cellnta::Cell::State m_newState = 0;
};

}  // namespace Ui
