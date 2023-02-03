#pragma once

#include <ActionBase.h>

namespace Ui {
namespace Action {
namespace Scene {

class ToggleFocus : public Action::Base {
 public:
  void Execute() override;
};

class LeaveFocus : public Action::Base {
 public:
  void Execute() override;
};

class EnterFocus : public Action::Base {
 public:
  void Execute() override;
};

}  // namespace Scene
}  // namespace Action
}  // namespace Ui
