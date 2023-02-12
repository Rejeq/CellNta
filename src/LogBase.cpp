#include "Cellnta/Log.h"

#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#endif

using namespace Cellnta;

std::unique_ptr<spdlog::logger> LogBase::InitDefault(const char* name) {
  std::unique_ptr<spdlog::logger> logger =
      std::make_unique<spdlog::logger>(name);
  std::vector<spdlog::sink_ptr> sinks;

#if defined(__ANDROID__)
  sinks.emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>(name));
#else
  sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

  if (AddSinks(logger.get(), sinks)) {
    assert(false && "Unable to set default logger sinks");
    return nullptr;
  }
  logger->set_pattern("[%H:%M:%S.%e] [%n] [%^%l%$] %v");

  return logger;
}

bool LogBase::AddSinks(spdlog::logger* logger,
                       const std::vector<spdlog::sink_ptr>& sinks) {
  if (logger == nullptr) {
    assert(logger != nullptr && "Logger is not initialized");
    return true;
  }

  auto& logSinks = logger->sinks();
  logSinks.insert(logSinks.end(), sinks.begin(), sinks.end());

  return false;
}
