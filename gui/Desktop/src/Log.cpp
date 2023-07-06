#include "Log.h"

static std::unique_ptr<Cellnta::LogBase> s_logger;

Cellnta::LogBase& Log::Get() {
  assert(s_logger != nullptr && "Desktop logger is not created");
  return *s_logger;
}

Cellnta::LogBase* Log::GetPtr() {
  return s_logger.get();
}

Cellnta::LogBase* Log::Create() {
  assert(s_logger == nullptr && "Desktop logger is already created");

  s_logger = std::make_unique<Cellnta::LogBase>("Desktop");
  return s_logger.get();
}

Cellnta::LogBase* Log::CreateDefault() {
  if (Create() == nullptr)
    return nullptr;

  if (s_logger->SetDefault())
    return nullptr;

  return s_logger.get();
}

static std::unique_ptr<Cellnta::LogBase> s_loggerGL;

Cellnta::LogBase& LogGL::Get() {
  assert(s_loggerGL != nullptr && "GL logger is not created");
  return *s_loggerGL;
}

Cellnta::LogBase* LogGL::GetPtr() {
  return s_loggerGL.get();
}

Cellnta::LogBase* LogGL::Create() {
  assert(s_loggerGL == nullptr && "GL logger is already created");

  s_loggerGL = std::make_unique<Cellnta::LogBase>("Desktop");
  return s_loggerGL.get();
}

Cellnta::LogBase* LogGL::CreateDefault() {
  if (Create() == nullptr)
    return nullptr;

  if (s_loggerGL->SetDefault())
    return nullptr;

  return s_loggerGL.get();
}
