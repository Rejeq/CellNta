#include "Cellnta/Renderer/GlBackend.h"

#include "Cellnta/Log.h"

using namespace Cellnta;

float* Cellnta::BeginArrayBufferSource(int offset, int size) {
  CELLNTA_PROFILE;

  float* dst =
      (float*)glMapBufferRange(GL_ARRAY_BUFFER, offset, size, GL_MAP_WRITE_BIT);

  if (dst == nullptr)
    CELLNTA_LOG_ERROR("Unable to map OpenGL buffer");

  return dst;
}

void Cellnta::EndArrayBufferSource() {
  CELLNTA_PROFILE;

  glUnmapBuffer(GL_ARRAY_BUFFER);
}
