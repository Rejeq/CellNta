#include "Cellnta/Log.h"

#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>
#if defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

using namespace Cellnta;

static std::unique_ptr<spdlog::logger> s_logger;

bool Log::InitDefault() {
  std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
  sinks.emplace_back(
      std::make_shared<spdlog::sinks::android_sink_mt>("CellNta"));
#else
  sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

  s_logger =
      std::make_unique<spdlog::logger>("CellNta", sinks.begin(), sinks.end());
  if (s_logger == nullptr)
    return true;

  s_logger->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

  return false;
}

bool Log::InitCustom(const std::vector<spdlog::sink_ptr>& sinks) {
  s_logger =
      std::make_unique<spdlog::logger>("CellNta", sinks.begin(), sinks.end());

  return (s_logger == nullptr);
}

spdlog::logger* Log::GetLogger() {
  assert(s_logger != nullptr && "Logger is not initialized");
  return s_logger.get();
}
