#pragma once

#include "Cellnta/Renderer/GlBackend.h"
#include "Cellnta/Renderer/NCellStorage.h"

namespace Cellnta {

class NCellDrawer {
 public:
  NCellDrawer();
  ~NCellDrawer();

  void Update(const NCellStorage& cells);

  GLuint GetBuffer() const { return m_buffer; }
  size_t GetSize() const { return m_size; }

 private:
  GLuint m_buffer = 0;
  size_t m_size = 0;
  size_t m_capacity = 0;
};

}  // namespace Cellnta
