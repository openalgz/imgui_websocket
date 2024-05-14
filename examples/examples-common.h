/*! \file common.h
 *  \brief Enter description here.
 */

#pragma once

#include <chrono>
#include <concepts>
#include <thread>
#include <format>
#include <iostream>
#include <filesystem>

#if __has_include("imgui/imgui.h")
#  include "imgui/imgui.h"
#else
#  include "imgui.h"
#endif

#if __has_include("imgui-ws/imgui-ws.h")
#  include "imgui-ws/imgui-ws.h"
#else
#  include "imgui-ws.h"
#endif

template<typename T>
concept is_filesystem_path_or_string = std::is_same_v<std::decay_t<T>, std::filesystem::path> ||
std::is_same_v<std::decay_t<T>, std::string> ||
std::is_same_v<std::decay_t<T>, std::string_view>;

template<is_filesystem_path_or_string T>
inline bool resource_exists(const T& resource_path, const std::string_view runtime_name)
{
    namespace fs = std::filesystem;
    if (not fs::exists(resource_path))
    {
        std::cerr << "Resource path '" << resource_path << "' does not exist.\nExiting " << runtime_name << ".";
        return false;
    }
    return true;
}

inline ImGuiWS& start_imgui_ws(int argc, char ** argv, std::string http_root_dir , const std::string_view example_dir_name, const char* index_html_file_name, int port)
{
    namespace fs = std::filesystem;

    static ImGuiWS imguiWS;

    printf("Usage: %s [port] [http-root]\n", argv[0]);

    // Override the port or http root directory.
    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) http_root_dir = argv[2];

    auto resource_path = std::format("{}/{}/{}", http_root_dir, example_dir_name, index_html_file_name);
    resource_path = fs::absolute(resource_path).string();

    if (not fs::exists(resource_path)){
        std::cerr << std::format("Resource, '{}', could not be found!\nExiting '{}' startup.\n", resource_path, example_dir_name);
        exit(0);
    }

    std::cout << "\nurl:localhost:" << port << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    imguiWS.init(port, http_root_dir + std::string("/") + std::string(example_dir_name), { "", index_html_file_name });

    return imguiWS;
}

inline ImGuiWS& init_imgui_ws(int argc, char ** argv, std::string http_root_dir , const std::string_view example_dir_name, const char* index_html_file_name, int port)
{
    namespace fs = std::filesystem;

    static ImGuiWS imguiWS;

    printf("Usage: %s [port] [http-root]\n", argv[0]);

    // Override the port or http root directory.
    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) http_root_dir = argv[2];

    auto resource_path = std::format("{}/{}/{}", http_root_dir, example_dir_name, index_html_file_name);
    resource_path = fs::absolute(resource_path).string();

    if (not fs::exists(resource_path)){
        std::cerr << std::format("Resource, '{}', could not be found!\nExiting '{}' startup.\n", resource_path, example_dir_name);
        exit(0);
    }

    std::cout << "\nurl: localhost:" << port << std::endl;

    imguiWS.init(port, http_root_dir + std::string("/") + std::string(example_dir_name), { "", index_html_file_name });

    return imguiWS;
}

struct VSync {
    VSync(double rate_fps = 60.0) : tStep_us(1000000.0/rate_fps) {}

    uint64_t tStep_us;
    uint64_t tLast_us = t_us();
    uint64_t tNext_us = tLast_us + tStep_us;

    inline uint64_t t_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); // duh ..
    }

    inline void wait() {
        uint64_t tNow_us = t_us();
        while (tNow_us < tNext_us - 100) {
            std::this_thread::sleep_for(std::chrono::microseconds((uint64_t) (0.9*(tNext_us - tNow_us))));
            tNow_us = t_us();
        }

        tNext_us += tStep_us;
    }

    inline float delta_s() {
        uint64_t tNow_us = t_us();
        uint64_t res = tNow_us - tLast_us;
        tLast_us = tNow_us;
        return float(res)/1e6f;
    }
};
