assign_local_host_root_path()

add_executable(basic-sdl2 main.cpp)
target_include_directories(basic-sdl2 PRIVATE ..)
target_link_libraries(basic-sdl2 PRIVATE imgui-sdl2 imgui-ws)
