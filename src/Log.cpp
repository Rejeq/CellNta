#include "Cellnta/Log.h"

using namespace Cellnta;

static std::unique_ptr<spdlog::logger> s_logger;

bool Log::InitDefault() {
  s_logger = LogBase::InitDefault("CellNta");
  return (s_logger == nullptr);
}

bool Log::AddSinks(const std::vector<spdlog::sink_ptr>& sinks) {
  return LogBase::AddSinks(s_logger.get(), sinks);
}

spdlog::logger* Log::GetLogger() {
  assert(s_logger != nullptr && "Cellnta logger is not initialized");
  return s_logger.get();
}
