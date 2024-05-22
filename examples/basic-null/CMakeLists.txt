assign_local_host_root_path()

add_executable(basic-null main.cpp)
target_include_directories(basic-null PRIVATE ..)
target_link_libraries(basic-null PRIVATE imgui-ws)
