#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/chrono.h>

namespace Log
{
  void InitDefault();
  void InitCustom(const std::vector<spdlog::sink_ptr>& sinks);
  std::shared_ptr<spdlog::logger>& GetLogger();
}

#define LOG_CRITICAL(...) ::Log::GetLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) ::Log::GetLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Log::GetLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::Log::GetLogger()->info(__VA_ARGS__)
#define LOG_DEBUG(...) ::Log::GetLogger()->debug(__VA_ARGS__)
#define LOG_TRACE(...) ::Log::GetLogger()->trace(__VA_ARGS__)
