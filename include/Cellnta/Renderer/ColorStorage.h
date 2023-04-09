#pragma once

#include <cstddef>
#include <vector>

namespace Cellnta {

class ColorStorage {
public:
  using Type = float;
  constexpr static int SIZE = 4;

  ColorStorage();

  bool NeedUpdate() const { return m_needUpdate; }
  void Handled() { m_needUpdate = false; }

  void Generate(int maxIter, int polygons);
  void SetSeed(float seed) { m_seedHue = seed; }

  void Clear() { return m_data.clear(); m_needUpdate = true; }

  int GetSize() const { return m_data.size(); }

  const float* GetData() const { return m_data.data(); }

 private:
  void GenerateRandomRGBAColor(Type* dst, Type alpha);

  float m_seedHue = 0;

  float m_hue = 0;
  float m_saturation = 0.9f;  // 0.8f
  float m_brightness = 0.9f;  // 0.5f

  std::vector<Type> m_data;
  bool m_needUpdate = false;
};

}  // namespace Cellnta
