#pragma once

#include <cfloat>
#include <functional>

namespace Ui {
namespace Widget {

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
  std::function<void()> m_res;
  float m_init = 0;
  float m_remaining = FLT_MAX;
};

}  // namespace Widget
}  // namespace Ui
