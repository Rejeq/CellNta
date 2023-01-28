#pragma once

#include <vector>

namespace Ui {
namespace Widget {

// This method depends on the previous frame
// so do not use it in dynamic places
class RightAlign {
 public:
  void Begin(int right);
  void End();

  void Next();

 private:
  void SavePosition();
  void PushPosition();

  std::vector<int> m_pos;
  int m_currentIdx;
};

}  // namespace Widget
}  // namespace Ui
