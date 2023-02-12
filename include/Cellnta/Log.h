#pragma once

#include "Cellnta/LogBase.h"

namespace Cellnta {

class Log {
 public:
  static bool InitDefault();
  static bool AddSinks(const std::vector<spdlog::sink_ptr>& sinks);
  static spdlog::logger* GetLogger();
};

}  // namespace Cellnta

#define _CT_LOGGER ::Cellnta::Log::GetLogger()

#define CELLNTA_LOG_CRITICAL(...) _CELLNTA_LOGBASE_C(_CT_LOGGER, __VA_ARGS__)
#define CELLNTA_LOG_ERROR(...) _CELLNTA_LOGBASE_E(_CT_LOGGER, __VA_ARGS__)
#define CELLNTA_LOG_WARN(...) _CELLNTA_LOGBASE_W(_CT_LOGGER, __VA_ARGS__)
#define CELLNTA_LOG_INFO(...) _CELLNTA_LOGBASE_I(_CT_LOGGER, __VA_ARGS__)
#define CELLNTA_LOG_DEBUG(...) _CELLNTA_LOGBASE_D(_CT_LOGGER, __VA_ARGS__)
#define CELLNTA_LOG_TRACE(...) _CELLNTA_LOGBASE_T(_CT_LOGGER, __VA_ARGS__)
