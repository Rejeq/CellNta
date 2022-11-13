#pragma once

#include "Cellnta/Profiling.h"
//#include "Log.h"

#define CELLNTA_RENDERER_GL
#ifndef CELLNTA_RENDERER_GL
#  if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#    define CELLNTA_RENDERER_GLES3
#  endif
#endif

#ifndef CELLNTA_RENDERER_GLES3
#  define CELLNTA_RENDERER_GL
#endif

//TODO: Move
//#include <cstdint>
//typedef uint8_t state_t;
