#pragma once

#include <memory>

#include "Log.h"  // TODO: Maybe move around?

namespace Ui {

class Context;

namespace ActionHint {
  typedef enum : int {
    None = 0,
    NoUndo = 1 << 0,
  } ActionHint_t;
}

namespace Action {

class Base {
 public:
  virtual ~Base() = default;

  Base& SetContext(Context* ctx) {
    p_ctx = ctx;
    return *this;
  }

  virtual void Execute() = 0;

  virtual bool CanUndo() { return false; }
  virtual void Undo() {}

 protected:
  Context* p_ctx;
};

class UndoBase : public Action::Base {
 public:
  virtual ~UndoBase() = default;

  bool CanUndo() override { return true; }
};

using BasePtr = std::unique_ptr<Action::Base>;

template <typename ActionType, typename... Args>
inline BasePtr Make(Args&&... args) {
  return std::make_unique<ActionType>(std::forward<Args>(args)...);
}

// FMT_HAS_CONSTEVAL used because fmt::format_string can be not a consteval
#ifdef FMT_HAS_CONSTEVAL
#define DK_CONSTEXPR constexpr
#else
#define DK_CONSTEXPR inline
#endif

template <typename... Args>
DK_CONSTEXPR bool CheckLogErr(bool expr, fmt::format_string<Args...> fmt,
                              Args&&... args) {
  if (expr)
    DESKTOP_LOG_ERROR(fmt, std::forward<Args>(args)...);
  return expr;
}

template <typename... Args>
DK_CONSTEXPR bool CheckLogWarn(bool expr, fmt::format_string<Args...> fmt,
                               Args&&... args) {
  if (expr)
    DESKTOP_LOG_WARN(fmt, std::forward<Args>(args)...);
  return expr;
}

#define DESKTOP_ACTION_DERR_MSG "Unable to execute action "
#define DESKTOP_ACTION_DERR_MSG_FMT DESKTOP_ACTION_DERR_MSG "{}"

DK_CONSTEXPR bool CheckLogDErr(bool expr, const std::string_view& actionName) {
  return CheckLogErr(expr, DESKTOP_ACTION_DERR_MSG_FMT, actionName);
}

DK_CONSTEXPR bool CheckLogDWarn(bool expr, const std::string_view& actionName) {
  return CheckLogWarn(expr, DESKTOP_ACTION_DERR_MSG_FMT, actionName);
}

#undef DK_CONSTEXPR

}  // namespace Action
}  // namespace Ui
