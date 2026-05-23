cmake_minimum_required(VERSION 3.10)
project(RE4ModMenu)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Only build shared library
add_library(modmenu_dinput8 SHARED src/dinput8.def
    src/main.cpp
    src/dinput8.cpp
    src/hooks.cpp
    src/gui.cpp
    src/egg_spawner.cpp

    # ImGui
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_demo.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_win32.cpp
    vendor/imgui/backends/imgui_impl_dx9.cpp

    # MinHook
    vendor/minhook/src/buffer.c
    vendor/minhook/src/hook.c
    vendor/minhook/src/trampoline.c
    vendor/minhook/src/hde/hde32.c
    vendor/minhook/src/hde/hde64.c
)

target_include_directories(modmenu_dinput8 PRIVATE
    src
    vendor/imgui
    vendor/imgui/backends
    vendor/minhook/include
)

target_link_libraries(modmenu_dinput8
    d3d9
    dinput8
    dxguid dwmapi
)

# Set output name to dinput8.dll
set_target_properties(modmenu_dinput8 PROPERTIES
    PREFIX ""
    OUTPUT_NAME "dinput8"
    SUFFIX ".dll"
)
