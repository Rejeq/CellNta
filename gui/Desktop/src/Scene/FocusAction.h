#pragma once

#include <ActionBase.h>

namespace Ui::Action::Scene {

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

}  // namespace Ui::Action::Scene
