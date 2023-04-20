#pragma once

#include "Cellnta/Renderer/Drawer/GL/GL.h"
#include "Cellnta/Renderer/Drawer/Color.h"

namespace Cellnta {

class ColorDrawerGL : public ColorDrawer {
 public:
  ColorDrawerGL();
  ~ColorDrawerGL();

  void Update(ColorStorage& color) override;

  GLuint GetBuffer() const { return m_buffer; }
  GLuint GetTexture() const { return m_texture; }

 private:
  GLuint m_buffer = 0;
  GLuint m_texture = 0;
};

}  // namespace Cellnta
