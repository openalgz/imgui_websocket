cmake_minimum_required (VERSION 3.10)

project(demo-ws)



set(http_root_dir "\"${CMAKE_CURRENT_SOURCE_DIR}\"")
set(template_path "${CMAKE_CURRENT_SOURCE_DIR}/project-demo.in.hpp")
set(output_path "${CMAKE_CURRENT_SOURCE_DIR}/project-demo.hpp")
configure_file("${template_path}" "${output_path}" @ONLY)

add_executable(demo-ws main.cpp)
target_include_directories(demo-ws PRIVATE .)
target_link_libraries(demo-ws PRIVATE imgui-ws)