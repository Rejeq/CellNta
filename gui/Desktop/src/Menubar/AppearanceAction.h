#pragma once

#include "ActionBase.h"

namespace Ui::Action::Appearance {

class ToggleFullscreen : public Action::Base {
 public:
  void Execute() override;
};

class ZoomOut : public Action::Base {
 public:
  void Execute() override;
};

class ZoomIn : public Action::Base {
 public:
  void Execute() override;
};

class ResetZoom : public Action::Base {
 public:
  void Execute() override;
};

}  // namespace Ui::Action::Appearance

