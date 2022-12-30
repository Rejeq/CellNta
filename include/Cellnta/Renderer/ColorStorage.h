#pragma once

#include <cstddef>
#include <vector>

#include "Cellnta/Renderer/GlBackend.h"

namespace Cellnta {

class ColorStorage {
  using Type = float;
  constexpr static int SIZE = 4;

public:
  ColorStorage();
  ~ColorStorage();

  void Generate(int maxIter, int polygons);
  void SetSeed(float seed) { m_seedHue = seed; }

  void Clear() { return m_data.clear(); }

  int GetSize() const { return m_data.size(); }

  GLuint GetTexture() const { return m_textureId; }
  const float* GetData() const { return m_data.data(); }

 private:
  void GenerateRandomRGBAColor(Type* dst, Type alpha);

  void UpdateColorBuffer();

  float m_seedHue = 0;

  float m_hue = 0;
  float m_saturation = 0.9f;  // 0.8f
  float m_brightness = 0.9f;  // 0.5f

  std::vector<Type> m_data;

  GLuint m_colorBuffer = 0;
  GLuint m_textureId = 0;
};

}  // namespace Cellnta
