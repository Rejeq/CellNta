#pragma once

#include <Cellnta/Config.h>

#ifdef CELLNTA_RENDERER_GLES3
#define IMGUI_IMPL_OPENGL_ES3
#endif

#include <Cellnta/Renderer/Drawer/GL/GL.h>
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DEFINE_MATH_OPERATORS

#define IM_VEC2_CLASS_EXTRA                                \
  inline bool operator==(const ImVec2& r) const noexcept { \
    return x == r.x && y == r.y;                           \
  }                                                        \
  inline bool operator!=(const ImVec2& r) const noexcept { \
    return !(*this == r);                                  \
  }

#define IM_VEC4_CLASS_EXTRA                                \
  inline bool operator==(const ImVec4& r) const noexcept { \
    return x == r.x && y == r.y && z == r.z && w == r.w;   \
  }                                                        \
  inline bool operator!=(const ImVec4& r) const noexcept { \
    return !(*this == r);                                  \
  }
