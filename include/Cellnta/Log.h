#pragma once

#ifdef CELLNTA_TRACE_LOGGING_ENABLED
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#endif

#include <vector>

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

namespace Cellnta::Log {
bool InitDefault();
bool InitCustom(const std::vector<spdlog::sink_ptr>& sinks);
spdlog::logger* GetLogger();

}  // namespace Cellnta::Log


#define CELLNTA_LOG_CRITICAL(...) \
  SPDLOG_LOGGER_CRITICAL(::Cellnta::Log::GetLogger(), __VA_ARGS__)
#define CELLNTA_LOG_ERROR(...) \
  SPDLOG_LOGGER_ERROR(::Cellnta::Log::GetLogger(), __VA_ARGS__)
#define CELLNTA_LOG_WARN(...) \
  SPDLOG_LOGGER_WARN(::Cellnta::Log::GetLogger(), __VA_ARGS__)
#define CELLNTA_LOG_INFO(...) \
  SPDLOG_LOGGER_INFO(::Cellnta::Log::GetLogger(), __VA_ARGS__)
#define CELLNTA_LOG_DEBUG(...) \
  SPDLOG_LOGGER_DEBUG(::Cellnta::Log::GetLogger(), __VA_ARGS__)
#define CELLNTA_LOG_TRACE(...) \
  SPDLOG_LOGGER_TRACE(::Cellnta::Log::GetLogger(), __VA_ARGS__)
