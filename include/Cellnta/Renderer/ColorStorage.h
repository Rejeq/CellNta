#pragma once

#include <cstddef>
#include <vector>

namespace Cellnta {

class ColorStorage
{
public:
  using color_t = float;
  constexpr static size_t SIZE = 4;

  void Generate(size_t maxIter, size_t polygons);
  void SetSeed(float seed) { m_seedHue = seed; }

  void Clear() { return m_data.clear(); }

  size_t GetSize() const { return m_data.size(); }
  size_t GetSizeInBytes() const { return GetSize() * sizeof(color_t); }

  const float* GetData() const { return m_data.data(); }
private:
  void GenerateRandomRGBAColor(color_t* dst, color_t alpha);

  float m_seedHue = 0;

  float m_hue = 0;
  float m_saturation = 0.9f;//0.8f
  float m_brightness = 0.9f;//0.5f

  std::vector<color_t> m_data;
};

} //namespace Cellnta
