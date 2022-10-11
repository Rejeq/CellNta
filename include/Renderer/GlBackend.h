#pragma once

#if defined(CELLNTA_RENDERER_GLES3)
#  include <GLES3/gl32.h>
#elif defined(CELLNTA_RENDERER_GL)
#  include <GL/glew.h>
#  define CELLNTA_HAS_GEOMETRY_SHADER
#else
#  error "Unable to determine renderer"
#endif
