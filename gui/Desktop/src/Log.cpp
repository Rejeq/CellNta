#include "Log.h"

static std::unique_ptr<spdlog::logger> s_logger;

bool Log::InitDefault() {
  s_logger = Cellnta::LogBase::InitDefault("Desktop");
  return (s_logger == nullptr);
}

bool Log::AddSinks(const std::vector<spdlog::sink_ptr>& sinks) {
  return Cellnta::LogBase::AddSinks(s_logger.get(), sinks);
}

spdlog::logger* Log::GetLogger() {
  assert(s_logger != nullptr && "Desktop logger is not initialized");
  return s_logger.get();
}

static std::unique_ptr<spdlog::logger> s_loggerGL;

bool LogGL::InitDefault() {
  s_loggerGL = Cellnta::LogBase::InitDefault("GL");
  return (s_loggerGL == nullptr);
}

bool LogGL::AddSinks(const std::vector<spdlog::sink_ptr>& sinks) {
  return Cellnta::LogBase::AddSinks(s_loggerGL.get(), sinks);
}

spdlog::logger* LogGL::GetLogger() {
  assert(s_loggerGL != nullptr && "GL logger is not initialized");
  return s_loggerGL.get();
}
