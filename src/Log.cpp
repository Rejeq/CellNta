#include "Cellnta/Log.h"

#include <memory>

using namespace Cellnta;
using namespace Cellnta::Log;

static std::unique_ptr<LogBase> s_logger;

LogBase& Log::Get() {
  assert(s_logger != nullptr && "Cellnta logger is not created");
  return *s_logger;
}

LogBase* Log::GetPtr() {
  return s_logger.get();
}

LogBase* Log::Create() {
  assert(s_logger == nullptr && "Logger is already defined");

  s_logger = std::make_unique<LogBase>("CellNta");
  return s_logger.get();
}

LogBase* Log::CreateDefault() {
  if (Create() == nullptr)
    return nullptr;

  if (s_logger->SetDefault())
    return nullptr;

  return s_logger.get();
}
