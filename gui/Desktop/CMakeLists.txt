set(GUI_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(THIRD_PARTY "${GUI_DIR}/third_party")

set(SDL_DIR   "${THIRD_PARTY}/SDL2/")
set(TRACY_DIR "${THIRD_PARTY}/tracy/")

set(RESOURCE_DIR "${CELLNTA_DIR}/Resource/")
#set(RESOURCE_DIR "/storage/emulated/0/projects/tmp/Resource/")

#Delete this
link_directories("F:/Libs/SDL2-2.0.22/build/RelWithDebInfo/")
link_directories("${CELLNTA_THIRD_PARTY}/glew/lib/Release/x64")

include("${GUI_DIR}/ImGui.cmake")

file(GLOB SOURCE_LIST
  ${GUI_DIR}/src/*.h
  ${GUI_DIR}/src/*.cpp
)
set(IMGUI_CONFIG "${GUI_DIR}/src/imgui_config.h")


if(1)
  add_executable(GUI ${SOURCE_LIST})

  target_include_directories(GUI PRIVATE
    ${SDL_DIR}/include
  )
  list(APPEND TARGET_LIBS
    CellNta
    imgui
  )

  if(WIN32)
    list(APPEND TARGET_LIBS
      SDL2
      SDL2main
      glew32
      opengl32
    )
  endif()

  if (UNIX)
      list(APPEND TARGET_LIBS
        SDL2
        GL
        GLEW
        dl
      )
  endif()

  target_compile_features(GUI PRIVATE ${CXX_ACTIVE_STANDART})

  #message("Libs: ${TARGET_LIBS}")
  target_link_libraries(GUI PUBLIC ${TARGET_LIBS})
else()
  target_sources(CellNta PRIVATE
    ${SOURCE_LIST}
    ${IMGUI_SRC}
  )
  target_include_directories(CellNta PUBLIC
    ${SDL_DIR}/include/
    ${IMGUI_DIR}/
  )

  target_compile_options(CellNta PRIVATE
    -fPIC
    -fcolor-diagnostics
  )

  target_link_options(CellNta PRIVATE
    -Wl,--no-undefined
    -Wl,-z,noexecstack
  )

  target_compile_options(CellNta PUBLIC "$<$<CONFIG:RELEASE>:-O3>")

  list(APPEND TARGET_LIBS SDL2 GLESv3 hidapi dl log)
  target_link_libraries(CellNta PUBLIC ${TARGET_LIBS})
endif()

target_compile_definitions(CellNta PUBLIC
  CELLNTA_RESOURCE_DIR="${RESOURCE_DIR}"
  IMGUI_USER_CONFIG="${IMGUI_CONFIG}"
)

add_compile_options("$<$<CONFIG:RELEASE>:-O3>")

add_compile_options("$<$<CONFIG:DEBUG>:-g3 -O0>")

