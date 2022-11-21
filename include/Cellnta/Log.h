#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>

namespace Cellnta {
namespace Log {

void InitDefault();
void InitCustom(const std::vector<spdlog::sink_ptr>& sinks);
std::shared_ptr<spdlog::logger>& GetLogger();

}
}

#define CELLNTA_LOG_CRITICAL(...) ::Cellnta::Log::GetLogger()->critical(__VA_ARGS__)
#define CELLNTA_LOG_ERROR(...) ::Cellnta::Log::GetLogger()->error(__VA_ARGS__)
#define CELLNTA_LOG_WARN(...) ::Cellnta::Log::GetLogger()->warn(__VA_ARGS__)
#define CELLNTA_LOG_INFO(...) ::Cellnta::Log::GetLogger()->info(__VA_ARGS__)
#define CELLNTA_LOG_DEBUG(...) ::Cellnta::Log::GetLogger()->debug(__VA_ARGS__)
#define CELLNTA_LOG_TRACE(...) ::Cellnta::Log::GetLogger()->trace(__VA_ARGS__)
