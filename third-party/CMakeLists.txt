if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
else()
    message("Warning imgui-ws is not supported on Windows!")
endif()

if (MSVC)
    # Suppress warning C4996 (deprecated declarations) for Visual Studio
    add_compile_options(/wd4996)
    add_compile_options(/wd4100)

elseif (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    # Suppress warnings about deprecated OpenSSL functions for GCC and Clang
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wunused-parameter)

endif()

add_subdirectory(imgui)
add_subdirectory(incppect)

