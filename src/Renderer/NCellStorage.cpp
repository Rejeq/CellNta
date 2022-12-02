#include "Cellnta/Renderer/NCellStorage.h"

#include "Cellnta/Config.h"

using namespace Cellnta;

void NCellStorage::Restore() {
  CELLNTA_PROFILE;
  m_cells = m_origCells;
}
