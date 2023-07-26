#include "Cellnta/Renderer/ShuffleAxisStorage.h"

#include <cassert>

using namespace Cellnta;

void ShuffleAxisStorage::SetShuffleAxis(Axis axis, Axis with) {
  assert(axis >= 0 && axis < (int)m_shuffleData.size() &&
         "Axis must be in dimension bound");
  assert(with >= 0 && with < (int)m_shuffleData.size() &&
         "Axis must be in dimension bound");

  m_shuffleData[axis] = with;
  if (axis != with)
    m_isShuffled = true;
  OnShuffleAxisChanged();
}

void ShuffleAxisStorage::ResetAxisShuffle() {
  for (size_t i = 0; i < m_shuffleData.size(); i++) {
    m_shuffleData[i] = i;
  }

  m_isShuffled = false;
  OnShuffleAxisChanged();
}

ShuffleAxisStorage::Axis ShuffleAxisStorage::GetShuffleAxis(Axis axis) const {
  assert(axis >= 0 && axis < (int)m_shuffleData.size() &&
         "Axis must be in dimension bound");

  return m_shuffleData[axis];
}

void ShuffleAxisStorage::SetDimension(int dim) {
  size_t oldSize = m_shuffleData.size();
  m_shuffleData.resize(dim);
  for (size_t i = oldSize; i < m_shuffleData.size(); i++) {
    m_shuffleData[i] = i;
  }
}
