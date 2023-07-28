#pragma once

#include <cassert>
#include <vector>

namespace Cellnta {

class ShuffleAxisStorage {
 public:
  using Axis = int;

  void SetShuffleAxis(Axis shuffle, Axis with);
  void ResetAxisShuffle();

  Axis GetShuffleAxis(Axis axis) const;
  bool IsAxisShuffled() const { return m_isShuffled; }
  int GetDimension() const { return m_shuffleData.size(); }

  // TODO: Move to other place, its redundant for this class
  void SetShuffleOnDraw(bool state) {
    if (state != m_needShuffleOnDraw)
      OnShuffleAxisChanged();
    m_needShuffleOnDraw = state;
  }
  bool NeedShuffleOnDraw() const { return m_needShuffleOnDraw; }

 protected:
  virtual void OnShuffleAxisChanged() = 0;

  void SetDimension(int dim);

  template <typename T>
  void FillShuffled(T* dst, const T* src, size_t size) const {
    if (!m_isShuffled) {
      memcpy(dst, src, size * sizeof(T));
      return;
    }

    const size_t shuffleSize = m_shuffleData.size();

    assert(shuffleSize <= size);
    for (size_t i = 0; i < shuffleSize; i++)
      *(dst + i) = src[m_shuffleData[i]];

    // Homogeneous axis leave as it is, if exist
    memcpy(dst + shuffleSize, src + shuffleSize,
           (size - shuffleSize) * sizeof(T));
  }

 private:
  std::vector<Axis> m_shuffleData;
  bool m_isShuffled = false;
  bool m_needShuffleOnDraw = true;
};

}  // namespace Cellnta
