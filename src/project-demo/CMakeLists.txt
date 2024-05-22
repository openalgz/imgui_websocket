cmake_minimum_required (VERSION 3.10)

project(example-projects-setup)

include(cmake/imgui-websocket.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

fetch_imgui_websocket_library()

add_subdirectory(demo-ws)