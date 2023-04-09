#include "Cellnta/Renderer/Drawer/Color.h"

using namespace Cellnta;

ColorDrawer::ColorDrawer() {
  glGenBuffers(1, &m_buffer);
  glGenTextures(1, &m_texture);

  glBindBuffer(GL_TEXTURE_BUFFER, m_buffer);
  glBindTexture(GL_TEXTURE_BUFFER, m_texture);
  static_assert(std::is_same<ColorStorage::Type, float>::value,
                "glTexBuffer has another type");
  static_assert(ColorStorage::SIZE == 4, "glTexBuffer has another size");
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, m_buffer);

}

ColorDrawer::~ColorDrawer() {
  if (m_buffer != 0)
    glDeleteBuffers(1, &m_buffer);
  if (m_texture != 0)
    glDeleteTextures(1, &m_texture);
}

void ColorDrawer::Update(ColorStorage& color) {
  glBindBuffer(GL_TEXTURE_BUFFER, m_buffer);
  glBufferData(GL_TEXTURE_BUFFER, color.GetSize() * sizeof(ColorStorage::Type),
               color.GetData(), GL_DYNAMIC_DRAW);
}

