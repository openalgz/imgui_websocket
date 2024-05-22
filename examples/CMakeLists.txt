macro(assign_local_host_root_path)
    set(http_root_dir "\"${CMAKE_CURRENT_SOURCE_DIR}\"")
    set(template_path "${CMAKE_CURRENT_SOURCE_DIR}/../localhost-root-path.in.hpp")
    get_filename_component(template_path "${template_path}" ABSOLUTE)
    set(output_path "${CMAKE_CURRENT_SOURCE_DIR}/localhost-root-path.hpp")
    configure_file("${template_path}" "${output_path}" @ONLY)
endmacro()


#if (IMGUI_WS_BUILD_EXAMPLES_NULL)
    add_subdirectory(basic-null)
    add_subdirectory(demo-null)
    add_subdirectory(textures-null)
#endif()

if (IMGUI_WS_SUPPORT_SDL2 AND IMGUI_WS_BUILD_EXAMPLES_SDL2)
    add_subdirectory(basic-sdl2)
endif()
