find_package(SDL2 CONFIG)

if (NOT SDL2_FOUND AND CELLNTA_FETCHING)
  set(SDL2_URL "https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.26.2.tar.gz")
  message(STATUS "Loading SDL2 from ${SDL2_URL}")

  include(FetchContent)
  FetchContent_Declare(SDL2
    URL ${SDL2_URL}
  )

  set(SDL_TEST              OFF CACHE BOOL "")
  set(SDL_SHARED            ON  CACHE BOOL "")
  set(SDL_STATIC            OFF CACHE BOOL "")
  set(SDL2_DISABLE_SDL2MAIN ON  CACHE BOOL "")

  # We need only OpenGL
  set(SDL_OPENGL       ON  CACHE BOOL "")
  set(SDL_OPENGLES     ON  CACHE BOOL "")
  set(SDL_COCOA        OFF CACHE BOOL "")
  set(SDL_DIRECTX      OFF CACHE BOOL "")
  set(SDL_RENDER_D3D   OFF CACHE BOOL "")
  set(SDL_RENDER_METAL OFF CACHE BOOL "")
  set(SDL_METAL        OFF CACHE BOOL "")

  # Disable unused subsystems
  set(SDL_AUDIO        OFF CACHE BOOL "")
  set(SDL_HIDAPI       OFF CACHE BOOL "")
  set(SDL_JOYSTICK     OFF CACHE BOOL "")
  set(SDL_HAPTIC       OFF CACHE BOOL "")
  set(SDL_LOCALE       OFF CACHE BOOL "")
  set(SDL_SENSOR       OFF CACHE BOOL "")
  set(SDL_RENDER       OFF CACHE BOOL "")

  FetchContent_MakeAvailable(SDL2)

  message(STATUS "SDL2 configured successfully")
endif()

if (NOT TARGET SDL2)
  message(STATUS "Not found SDL2 target")

  add_library(SDL2 INTERFACE)
  target_include_directories(SDL2 INTERFACE ${SDL2_INCLUDE_DIRS})
  target_link_libraries(SDL2 INTERFACE ${SDL2_LIBRARIES})
endif()

if (NOT TARGET SDL2::SDL2)
  message(STATUS "Not found SDL2::SDL2 alias target")

  add_library(SDL2::SDL2 ALIAS SDL2)
endif()
