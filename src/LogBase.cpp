#include "Cellnta/Log.h"

#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

using namespace Cellnta;

// TODO: Delete this when ability to change pattern is added
const char* s_defaultPattern = "[%H:%M:%S.%e] [%n] [%^%l%$] %v";

bool LogBase::SetDefault() {
  bool err = false;

  err = SetDefaultSinks();
  SetDefaultPattern();
  SetDefaultLevel();
  return err;
}

bool LogBase::SetDefaultSinks() {
  bool err = false;
#if defined(__ANDROID__)
  err |= AddSink<spdlog::sinks::android_sink_mt>(name);
#else
  err |= AddSink<spdlog::sinks::stdout_color_sink_mt>();
#endif
  return err;
}

void LogBase::SetDefaultPattern() {
  p_logger.set_pattern(s_defaultPattern);
}

const char* LogBase::GetCurrentPattern() {
  return s_defaultPattern;
}

void LogBase::SetLevel(LogBase::Level level) {
  p_logger.set_level((spdlog::level::level_enum)level);
}

void LogBase::SetDefaultLevel() {
  SetLevel(GetDefaultLevel());
}

LogBase::Level LogBase::GetDefaultLevel() {
  return Level::Info;
}

spdlog::logger* LogBase::GetLogger() {
  return &p_logger;
}
