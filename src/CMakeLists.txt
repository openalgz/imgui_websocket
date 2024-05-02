set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # Suppress warning C4996 (deprecated declarations) for Visual Studio
    add_compile_options(/wd4996)
elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    # Suppress warnings about deprecated OpenSSL functions for GCC and Clang
    add_compile_options(-Wno-deprecated-declarations)
endif()

if (IMGUI_WS_ALL_WARNINGS)
    if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    else()
        message("Warning imgui-ws is not supported on Windows!")
    endif()
endif()

if (UNIX)
    add_compile_options(-Wno-sign-compare)
endif()

file(READ "imgui-ws.js" src_imgui-ws_js)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/common.h.in ${CMAKE_CURRENT_SOURCE_DIR}/common.h @ONLY)

add_library(imgui-ws ${IMTUI_LIBRARY_TYPE}
    imgui-ws.cpp
    compressor-xor-rle-per-draw-list.cpp
    compressor-xor-rle-per-draw-list-with-vtx-offset.cpp
    )

target_include_directories(imgui-ws PUBLIC
    ../include
    ../third-party/incppect/include
    "${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/src"
    "${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/uSockets/src"
    )

target_link_libraries(imgui-ws PRIVATE
    imgui
    incppect::incppect
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
    )
