if (APPLE)
    set(ADDITIONAL_LIBRARIES "-framework Cocoa")
else (APPLE)
    unset(ADDITIONAL_LIBRARIES)
endif (APPLE)

if (MSVC)
    # Suppress warning C4996 (deprecated declarations) for Visual Studio
    add_compile_options(/wd4996)
    add_compile_options(/wd4100)

elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    # Suppress warnings about deprecated OpenSSL functions for GCC and Clang
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wunused-parameter)

endif()

add_library(imgui
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    )

target_include_directories(imgui INTERFACE
    .
    )

target_include_directories(imgui PRIVATE
    imgui
    )

target_link_libraries(imgui PRIVATE
    ${ADDITIONAL_LIBRARIES}
    )

target_compile_definitions(imgui PUBLIC
    IMGUI_USER_CONFIG="../imgui-extra/imconfig-vtx32.h"
    )

if (IMGUI_WS_SUPPORT_SDL2)
    add_library(imgui-sdl2
        imgui-extra/imgui_impl.cpp
        imgui/backends/imgui_impl_sdl.cpp
        imgui/backends/imgui_impl_opengl3.cpp
        )

    target_include_directories(imgui-sdl2 PUBLIC
        ${SDL2_INCLUDE_DIRS}
        )

    target_include_directories(imgui-sdl2 PRIVATE
        imgui
        imgui-extra
        )

    target_link_libraries(imgui-sdl2 PUBLIC
        imgui
        ${OPENGL_LIBRARIES}
        ${SDL2_LIBRARIES}
        )

    target_link_libraries(imgui-sdl2 PRIVATE
        ${CMAKE_DL_LIBS}
        ${ADDITIONAL_LIBRARIES}
        )
endif()
