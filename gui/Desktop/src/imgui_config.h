#pragma once

#include <Config.h>

#ifdef CELLNTA_RENDERER_GLES3
#define IMGUI_IMPL_OPENGL_ES3
#endif

#include <GlBackend.h>
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
