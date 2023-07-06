#pragma once

#include <Cellnta/LogBase.h>

class Log {
 public:
  static Cellnta::LogBase& Get();
  static Cellnta::LogBase* GetPtr();
  static Cellnta::LogBase* Create();
  static Cellnta::LogBase* CreateDefault();
};

class LogGL {
 public:
  static Cellnta::LogBase& Get();
  static Cellnta::LogBase* GetPtr();
  static Cellnta::LogBase* Create();
  static Cellnta::LogBase* CreateDefault();
};

#define _DK_LOGGER ::Log::Get()
#define DESKTOP_LOG_CRITICAL(...) _CELLNTA_LOGBASE_C(_DK_LOGGER, __VA_ARGS__)
#define DESKTOP_LOG_ERROR(...) _CELLNTA_LOGBASE_E(_DK_LOGGER, __VA_ARGS__)
#define DESKTOP_LOG_WARN(...) _CELLNTA_LOGBASE_W(_DK_LOGGER, __VA_ARGS__)
#define DESKTOP_LOG_INFO(...) _CELLNTA_LOGBASE_I(_DK_LOGGER, __VA_ARGS__)
#define DESKTOP_LOG_DEBUG(...) _CELLNTA_LOGBASE_D(_DK_LOGGER, __VA_ARGS__)
#define DESKTOP_LOG_TRACE(...) _CELLNTA_LOGBASE_T(_DK_LOGGER, __VA_ARGS__)

#define _GL_LOGGER ::LogGL::Get()
#define DESKTOP_GL_LOG_CRITICAL(...) _CELLNTA_LOGBASE_C(_GL_LOGGER, __VA_ARGS__)
#define DESKTOP_GL_LOG_ERROR(...) _CELLNTA_LOGBASE_E(_GL_LOGGER, __VA_ARGS__)
#define DESKTOP_GL_LOG_WARN(...) _CELLNTA_LOGBASE_W(_GL_LOGGER, __VA_ARGS__)
#define DESKTOP_GL_LOG_INFO(...) _CELLNTA_LOGBASE_I(_GL_LOGGER, __VA_ARGS__)
#define DESKTOP_GL_LOG_DEBUG(...) _CELLNTA_LOGBASE_D(_GL_LOGGER, __VA_ARGS__)
#define DESKTOP_GL_LOG_TRACE(...) _CELLNTA_LOGBASE_T(_GL_LOGGER, __VA_ARGS__)
