#pragma once

#include "Window.h"
#include <Cellnta/World/World.h>

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

  Cellnta::Cell m_setCell;
  Timer m_timer;
};

}  // namespace Ui
