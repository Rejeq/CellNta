#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#if defined(__ANDROID__)
# include "spdlog/sinks/android_sink.h"
#endif

namespace Log
{
  static std::shared_ptr<spdlog::logger> s_logger;

  void InitDefault()
  {
    std::vector<spdlog::sink_ptr> sinks;
#if defined(__ANDROID__)
    sinks.emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>("CellNta"));
#else
    sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif

    s_logger = std::make_shared<spdlog::logger>("CellNta", sinks.begin(), sinks.end());
  }

  void InitCustom(const std::vector<spdlog::sink_ptr>& sinks)
  {
    s_logger = std::make_shared<spdlog::logger>("CellNta", sinks.begin(), sinks.end());
  }

  std::shared_ptr<spdlog::logger>& GetLogger()
  {
    return s_logger;
  }
}
