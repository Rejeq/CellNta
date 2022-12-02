#pragma once

#include <memory>
#include <vector>

#include <spdlog/fmt/chrono.h>
#include <spdlog/spdlog.h>

namespace Cellnta::Log {

bool InitDefault();
bool InitCustom(const std::vector<spdlog::sink_ptr>& sinks);
std::shared_ptr<spdlog::logger>& GetLogger();

}  // namespace Cellnta::Log

#define CELLNTA_LOG_CRITICAL(...) \
  ::Cellnta::Log::GetLogger()->critical(__VA_ARGS__)
#define CELLNTA_LOG_ERROR(...) ::Cellnta::Log::GetLogger()->error(__VA_ARGS__)
#define CELLNTA_LOG_WARN(...) ::Cellnta::Log::GetLogger()->warn(__VA_ARGS__)
#define CELLNTA_LOG_INFO(...) ::Cellnta::Log::GetLogger()->info(__VA_ARGS__)
#define CELLNTA_LOG_DEBUG(...) ::Cellnta::Log::GetLogger()->debug(__VA_ARGS__)
#define CELLNTA_LOG_TRACE(...) ::Cellnta::Log::GetLogger()->trace(__VA_ARGS__)
