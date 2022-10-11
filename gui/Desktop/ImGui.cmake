set(IMGUI_DIR "${THIRD_PARTY}/imgui")

set(IMGUI_SRC
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui.cpp
)

add_library(imgui INTERFACE)
target_sources(imgui INTERFACE ${IMGUI_SRC})
target_include_directories(imgui INTERFACE ${IMGUI_DIR})
target_compile_features(imgui INTERFACE ${CXX_ACTIVE_STANDART})

