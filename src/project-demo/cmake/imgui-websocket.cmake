#[[
    Incude this cmake module in projects that require imgui-websockets.
    
    Example:

        include(cmake/imgui-websocket.cmake)

        fetch_imgui_websocket_library()
        .
        .
        .
        target_link_libraries(${MY_TARGET} PRIVATE imgui-ws)

#]]
cmake_minimum_required (VERSION 3.10)

macro(fetch_imgui_websocket_library)

    if(CMAKE_CXX_STANDARD LESS 20)
        message(STATUS 
            "\nWarning"
            "\nThe Imgui WebSocket Library requires C++ 20 or above."
            "\nThe recommened settings are:"              
            "\n set(CMAKE_CXX_STANDARD 20 or greater)"
            "\n set(CMAKE_CXX_STANDARD_REQUIRED ON)"
            "\n set(CMAKE_CXX_EXTENSIONS OFF)\n"
        )
    endif()

    include(FetchContent)

    option(IMGUI_WS_ALL_WARNINGS "Enable all compile time warnings when using GCC or Clang" OFF)
        
    FetchContent_Declare(
        "imgui-ws"
        GIT_REPOSITORY "https://github.com/openalgz/imgui_websocket.git"
        GIT_TAG "main"
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable("imgui-ws")

    if (IMGUI_WS_ALL_WARNINGS AND (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        #
        # Enable all warnings for GCC and Clang
        #
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
    elseif (IMGUI_WS_ALL_WARNINGS)
        message(WARNING "imgui-ws is not supported on Windows!")
    endif()

    # Suppress specified imgui and uWebSocket warnings.
    #
    if (MSVC)
        add_compile_options(/wd4996 /wd4100 /wd4267 /wd4996)
    else()
        # Suppress warnings about deprecated OpenSSL functions and unused parameters for GCC and Clang
        add_compile_options(
            -Wno-deprecated-declarations
            -Wno-sign-compare
            -Wno-unused-parameter
            -Wno-unused-variable       
                )
    endif()

endmacro()