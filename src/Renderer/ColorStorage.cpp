#include "Cellnta/Renderer/ColorStorage.h"

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstring>

#include "Cellnta/Config.h"

using namespace Cellnta;

void ColorStorage::Generate(int maxIter, int polygons) {
  CELLNTA_PROFILE;

  m_data.resize(polygons * maxIter * SIZE);
  Type* ptr = m_data.data();

  m_hue = m_seedHue;

  for (int i = 0; i < maxIter; ++i) {
    assert(ptr < m_data.data() + GetSize() && "Out of range");

    GenerateRandomRGBAColor(ptr, /*1.0f*/ 0.9f);
    if (polygons > 1) {
      if (polygons == 1)
        std::memcpy(ptr + SIZE, ptr, SIZE * sizeof(Type));
      else {
        for (int j = 1; j < polygons; ++j)
          std::memcpy(ptr + (SIZE * j), ptr, SIZE * sizeof(Type));
      }
    }
    ptr += polygons * SIZE;
  }
}

void ColorStorage::GenerateRandomRGBAColor(Type* dst, Type alpha) {
  CELLNTA_PROFILE;

  // https://martin.ankerl.com/2009/12/09/how-to-create-random-colors-programmatically/
  constexpr float golden_ratio_conjugate = 0.618033988749895f;

  m_hue += golden_ratio_conjugate;
  m_hue = std::fmod(m_hue, 1.0f);

  float r = 0;
  float g = 0;
  float b = 0;
  const float h = m_hue;
  const float s = m_saturation;
  const float v = m_brightness;

  int h_i = h * 6;
  float f = h * 6 - h_i;
  float p = v * (1 - s);
  float q = v * (1 - f * s);
  float t = v * (1 - (1 - f) * s);
  switch (h_i) {
    case 0:
      r = v;
      g = t;
      b = p;
      break;
    case 1:
      r = q;
      g = v;
      b = p;
      break;
    case 2:
      r = p;
      g = v;
      b = t;
      break;
    case 3:
      r = p;
      g = q;
      b = v;
      break;
    case 4:
      r = t;
      g = p;
      b = v;
      break;
    case 5:
      r = v;
      g = p;
      b = q;
      break;
    default: assert(0 && "Unreachable"); break;
  }

  dst[0] = r;
  dst[1] = g;
  dst[2] = b;
  dst[3] = alpha;
}
