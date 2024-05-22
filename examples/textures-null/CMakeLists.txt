assign_local_host_root_path()

add_executable(textures-null main.cpp)
target_include_directories(textures-null PRIVATE ..)
target_link_libraries(textures-null PRIVATE imgui-ws)
