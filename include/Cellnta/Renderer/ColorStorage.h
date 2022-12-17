#pragma once

#include <cstddef>
#include <vector>

namespace Cellnta {

class ColorStorage {
 public:
  using Type = float;
  constexpr static int SIZE = 4;

  void Generate(int maxIter, int polygons);
  void SetSeed(float seed) { m_seedHue = seed; }

  void Clear() { return m_data.clear(); }

  int GetSize() const { return m_data.size(); }
  int GetSizeInBytes() const { return GetSize() * sizeof(Type); }

  const float* GetData() const { return m_data.data(); }

 private:
  void GenerateRandomRGBAColor(Type* dst, Type alpha);

  float m_seedHue = 0;

  float m_hue = 0;
  float m_saturation = 0.9f;  // 0.8f
  float m_brightness = 0.9f;  // 0.5f

  std::vector<Type> m_data;
};

}  // namespace Cellnta
