#pragma once

#include <memory>

#include "Log.h"  // TODO: Maybe move around?

namespace Ui {

class Context;

namespace Action {

class Base {
 public:
  virtual ~Base() = default;

  Base& SetContext(Context* ctx) {
    p_ctx = ctx;
    return *this;
  }

  virtual void Execute() = 0;

 protected:
  Context* p_ctx;
};

using BasePtr = std::unique_ptr<Action::Base>;

template <class ActionType>
inline BasePtr Make(ActionType&& action) {
  return std::make_unique<ActionType>(std::move(action));
}

template <typename... Args>
constexpr bool CheckLogErr(bool expr, fmt::format_string<Args...> fmt,
                           Args&&... args) {
  if (expr)
    DESKTOP_LOG_ERROR(fmt, std::forward<Args>(args)...);
  return expr;
}

template <typename... Args>
constexpr bool CheckLogWarn(bool expr, fmt::format_string<Args...> fmt,
                            Args&&... args) {
  if (expr)
    DESKTOP_LOG_WARN(fmt, std::forward<Args>(args)...);
  return expr;
}

#define DESKTOP_ACTION_DERR_MSG "Unable to execute action "
#define DESKTOP_ACTION_DERR_MSG_FMT DESKTOP_ACTION_DERR_MSG "{}"

constexpr bool CheckLogDErr(bool expr, const std::string_view& actionName) {
  return CheckLogErr(expr, DESKTOP_ACTION_DERR_MSG_FMT, actionName);
}

constexpr bool CheckLogDWarn(bool expr, const std::string_view& actionName) {
  return CheckLogWarn(expr, DESKTOP_ACTION_DERR_MSG_FMT, actionName);
}

}  // namespace Action
}  // namespace Ui
