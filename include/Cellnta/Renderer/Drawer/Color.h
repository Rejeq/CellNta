#pragma once

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/ColorStorage.h"

namespace Cellnta {

class ColorDrawer {
 public:
  ColorDrawer();
  ~ColorDrawer();

  void Update(ColorStorage& color);

  GLuint GetBuffer() const { return m_buffer; }
  GLuint GetTexture() const { return m_texture; }

 private:
  GLuint m_buffer = 0;
  GLuint m_texture = 0;
};

}  // namespace Cellnta

