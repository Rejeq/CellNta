#pragma once

#ifdef SPDLOG_ACTIVE_LEVEL
#undef SPDLOG_ACTIVE_LEVEL
#endif

#ifdef CELLNTA_TRACE_LOGGING_ENABLED
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

namespace Cellnta {

using Logger = spdlog::logger;
using SinkPtr = spdlog::sink_ptr;

class LogBase {
 public:
  enum class Level {
    Trace = spdlog::level::trace,
    Debug = spdlog::level::debug,
    Info = spdlog::level::info,
    Warn = spdlog::level::warn,
    Error = spdlog::level::err,
    Critical = spdlog::level::critical,
    Off = spdlog::level::off,
  };

  LogBase(const char* name) : p_logger(spdlog::logger(name)) {}

  template <typename T, typename... Args>
  bool AddSink(Args&&... args) {
    auto sink = std::make_shared<T>(std::forward<Args>(args)...);
    if (sink == nullptr)
      return true;

    sink->set_pattern(GetCurrentPattern());
    sink->set_level((spdlog::level::level_enum)GetCurrentLevel());
    p_logger.sinks().push_back(sink);
    return false;
  }

  void SetLevel(Level level);

  bool SetDefault();
  bool SetDefaultSinks();
  void SetDefaultLevel();
  void SetDefaultPattern();

  Level GetDefaultLevel();
  Level GetCurrentLevel();
  const char* GetCurrentPattern();

  // Best if you never use this outside of macros
  spdlog::logger* GetLogger();

 protected:
  spdlog::logger p_logger;
};

}  // namespace Cellnta

#ifdef CELLNTA_LOGGING_ENABLED

// clang-format off
#define _CT_TO_SPDLOG(log) ((log).GetLogger())
#define _CELLNTA_LOGBASE_C(log, ...) SPDLOG_LOGGER_CRITICAL(_CT_TO_SPDLOG(log), __VA_ARGS__)
#define _CELLNTA_LOGBASE_E(log, ...) SPDLOG_LOGGER_ERROR(_CT_TO_SPDLOG(log), __VA_ARGS__)
#define _CELLNTA_LOGBASE_W(log, ...) SPDLOG_LOGGER_WARN(_CT_TO_SPDLOG(log), __VA_ARGS__)
#define _CELLNTA_LOGBASE_I(log, ...) SPDLOG_LOGGER_INFO(_CT_TO_SPDLOG(log), __VA_ARGS__)
#define _CELLNTA_LOGBASE_D(log, ...) SPDLOG_LOGGER_DEBUG(_CT_TO_SPDLOG(log), __VA_ARGS__)
#define _CELLNTA_LOGBASE_T(log, ...) SPDLOG_LOGGER_TRACE(_CT_TO_SPDLOG(log), __VA_ARGS__)
// clang-format on

#else

// clang-format off
#define _CELLNTA_LOGBASE_C(log, ...) do {} while(false)
#define _CELLNTA_LOGBASE_E(log, ...) do {} while(false)
#define _CELLNTA_LOGBASE_W(log, ...) do {} while(false)
#define _CELLNTA_LOGBASE_I(log, ...) do {} while(false)
#define _CELLNTA_LOGBASE_D(log, ...) do {} while(false)
#define _CELLNTA_LOGBASE_T(log, ...) do {} while(false)
// clang-format on

#endif  // CELLNTA_LOGGING_ENABLED
