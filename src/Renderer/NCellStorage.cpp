#include "Cellnta/Renderer/NCellStorage.h"

#include "Cellnta/Config.h"

using namespace Cellnta;

void NCellStorage::Restore()
{
  ProfileScope;
  m_cells = m_origCells;
}
