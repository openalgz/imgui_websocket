assign_local_host_root_path()

add_executable(demo-null main.cpp)
target_include_directories(demo-null PRIVATE ..)
target_link_libraries(demo-null PRIVATE imgui-ws)
add_imgui(demo-null)
