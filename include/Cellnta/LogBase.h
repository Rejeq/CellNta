#pragma once

#ifdef CELLNTA_TRACE_LOGGING_ENABLED
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <memory>

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

namespace Cellnta {

class LogBase {
 public:
  static std::unique_ptr<spdlog::logger> InitDefault(const char* name);
  static bool AddSinks(spdlog::logger* logger,
                       const std::vector<spdlog::sink_ptr>& sinks);
};

}  // namespace Cellnta

#define _CELLNTA_LOGBASE_C(log, ...) SPDLOG_LOGGER_CRITICAL(log, __VA_ARGS__)
#define _CELLNTA_LOGBASE_E(log, ...) SPDLOG_LOGGER_ERROR(log, __VA_ARGS__)
#define _CELLNTA_LOGBASE_W(log, ...) SPDLOG_LOGGER_WARN(log, __VA_ARGS__)
#define _CELLNTA_LOGBASE_I(log, ...) SPDLOG_LOGGER_INFO(log, __VA_ARGS__)
#define _CELLNTA_LOGBASE_D(log, ...) SPDLOG_LOGGER_DEBUG(log, __VA_ARGS__)
#define _CELLNTA_LOGBASE_T(log, ...) SPDLOG_LOGGER_TRACE(log, __VA_ARGS__)
