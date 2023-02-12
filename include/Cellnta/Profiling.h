#pragma once

#ifdef CELLNTA_PROFILING_ENABLED
#ifndef TRACY_ENABLE
#error "TRACY_ENABLE define not loaded"
#define TRACY_ENABLE
#endif
#include <Tracy.hpp>

#define CELLNTA_PROFILE ZoneScoped
#define CELLNTA_PROFILE_NAMED(name) ZoneScopedN(name)
#define CELLNTA_PROFILE_FRAME FrameMark
#else

// clang-format off
#define CELLNTA_PROFILE do { } while (false)
#define CELLNTA_PROFILE_NAMED(x) do { } while (false)
#define CELLNTA_PROFILE_FRAME do { } while (false)
// clang-format on

#endif /* CELLNTA_PROFILING_ENABLED */
