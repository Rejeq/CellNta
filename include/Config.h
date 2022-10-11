#pragma once

#ifndef CELLNTA_RESOURCE_DIR
#error "Not provided resource directory"
#define CELLNTA_RESOURCE_DIR
#endif

#include "Profiling.h"
#include "Log.h"

//#define CELLNTA_RENDERER_GLES3
#ifndef CELLNTA_RENDERER_GL
#  if defined(__EMSCRIPTEN__) || defined(__ANDROID__)
#    define CELLNTA_RENDERER_GLES3
#  endif
#endif

#ifndef CELLNTA_RENDERER_GLES3
#  define CELLNTA_RENDERER_GL
#endif

//TODO: Move
#include <cstdint>
typedef uint8_t state_t;
