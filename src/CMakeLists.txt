set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    # Suppress warning C4996 (deprecated declarations) for Visual Studio
    add_compile_options(/wd4996 /wd4100)

elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    # Suppress warnings about deprecated OpenSSL functions for GCC and Clang
    add_compile_options(
      -Wno-deprecated-declarations 
      -Wunused-parameter
      -Wno-sign-compare
    )

endif()

if (IMGUI_WS_ALL_WARNINGS AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # Enable all warnings for GCC and Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

elseif (IMGUI_WS_ALL_WARNINGS)
    message(WARNING "imgui-ws is not supported on Windows!")
endif()

option(@USE_UINT32 "Use imgui-ws UINT32 implementation; If this is false then UINT16s are used." TRUE)

if (@USE_UINT32)
    #add_compile_definitions("ImDrawIdx=unsigned int")
    file(READ "${IMGUI_WS_WORKING_DIR}/src/imgui-ws-int32.js" src_imgui-ws_js)
else()
    file(READ "${IMGUI_WS_WORKING_DIR}/src/imgui-ws-int16.js" src_imgui-ws_js)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/common.h.in ${CMAKE_CURRENT_SOURCE_DIR}/common.h @ONLY)

add_library(imgui-ws ${IMTUI_LIBRARY_TYPE}
    "${IMGUI_WS_WORKING_DIR}/src/imgui-ws.cpp"
    "${IMGUI_WS_WORKING_DIR}/src/compressor-xor-rle-per-draw-list.cpp"
    "${IMGUI_WS_WORKING_DIR}/src/compressor-xor-rle-per-draw-list-with-vtx-offset.cpp"
)

target_include_directories(imgui-ws PUBLIC
    "${IMGUI_WS_WORKING_DIR}/include"
    "${CMAKE_BINARY_DIR}/_deps/imgui-src"
    "${CMAKE_BINARY_DIR}/_deps/implot-src"
    "${CMAKE_BINARY_DIR}/_deps/incppect-src/include"
    "${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/src"
    "${CMAKE_BINARY_DIR}/_deps/uwebsockets-src/uSockets/src"
)

target_link_libraries(imgui-ws PRIVATE
    incppect::incppect
    ${CMAKE_DL_LIBS}
    ${CMAKE_THREAD_LIBS_INIT}
)

add_imgui(${PROJECT_NAME})

add_library(imguiws_imguiws INTERFACE)
target_link_libraries(imguiws_imguiws INTERFACE imgui-ws)
add_library(imguiws::imguiws ALIAS imguiws_imguiws)
