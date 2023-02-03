#pragma once

#include <memory>

namespace Ui {

class Context;

namespace Action {

class Base {
 public:
  virtual ~Base() = default;

  void SetContext(Context* ctx) { p_ctx = ctx; }

  virtual void Execute() = 0;

 protected:
  Context* p_ctx;
};

using BasePtr = std::unique_ptr<Action::Base>;

template <class ActionType>
inline BasePtr Make(ActionType&& action) {
  return std::make_unique<ActionType>(std::move(action));
}

}  // namespace Action
}  // namespace Ui
