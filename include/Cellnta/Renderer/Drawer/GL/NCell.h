#pragma once

#include "Cellnta/Renderer/Drawer/GL/GL.h"
#include "Cellnta/Renderer/Drawer/NCell.h"

namespace Cellnta {

class NCellDrawerGL : public NCellDrawer {
 public:
  NCellDrawerGL();
  ~NCellDrawerGL();

  void Update(const NCellStorage& cells) override;

  GLuint GetBuffer() const { return m_buffer; }
  size_t GetSize() const { return m_size; }

 private:
  GLuint m_buffer = 0;
  size_t m_size = 0;
  size_t m_capacity = 0;
};

}  // namespace Cellnta
