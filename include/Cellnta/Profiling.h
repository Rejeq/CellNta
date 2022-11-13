#pragma once

#ifdef PROFILING_ENABLE
# ifndef TRACY_ENABLE
# error "TRACY_ENABLE define not loaded"
# define TRACY_ENABLE
# endif
# include <Tracy.hpp>

# define ProfileScope ZoneScoped
# define ProfileScopeN(name) ZoneScopedN(name)
# define ProfileFrame FrameMark
# define ProfileFrameN(name) FrameMarkN(name)
#else
# define ProfileScope
# define ProfileScopeN(x)
# define ProfileFrame
# define ProfileFrameN(x)
#endif /* PROFILING_ENABLE */
