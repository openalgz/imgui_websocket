#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <map>
#include <ranges>
#include <thread>
#include <type_traits>
#include <vector>

#if __has_include("imgui/imgui.h")
#include "imgui/imgui.h"
#else
#include "imgui.h"
#endif

#if __has_include("imgui-ws/imgui-ws.h")
#include "imgui-ws/imgui-ws.h"
#else
#include "imgui-ws.h"
#endif

#include "localhost-root-path.hpp"

namespace demo
{
    template <typename StrT>
    concept is_string =
        std::is_same_v<std::decay_t<StrT>, std::string> || std::is_same_v<std::decay_t<StrT>, std::string_view> ||
        std::is_same_v<std::decay_t<StrT>, std::wstring> || std::is_same_v<std::decay_t<StrT>, std::wstring_view>;

    template <typename StrT_Or_PathT>
    concept is_filesystem_path_or_string =
        std::is_same_v<std::decay_t<StrT_Or_PathT>, std::filesystem::path> || is_string<StrT_Or_PathT>;

    inline auto check_localhost_path(int argc, char **argv, const std::string_view example_name, int port, const std::string_view index_html_file_name = "index.html")
    {
        namespace fs = std::filesystem;
        
        std::string http_root_dir = localhost_root_path;

        printf("\nUsage: %s [port] [http-root]\n", argv[0]);

        if (argc > 1)
            port = atoi(argv[1]);
        if (argc > 2)
            http_root_dir = argv[2];

        auto resource_path = std::format("{}/{}", http_root_dir, index_html_file_name);

        if (not fs::exists(resource_path))
        {
            std::cerr << std::format("Resource path, '{}', could not be found!\nExiting '{}' startup.\n", resource_path, example_name);
            exit(0);
        }

        std::cout << "\nurl: localhost:" << port << std::endl;

        return http_root_dir;
    }

    inline ImGuiWS &start_imgui_ws(int argc, char **argv, const std::string_view example_name, int port, const std::string_view index_html_file_name = "index.html")
    {
        static ImGuiWS imguiWS;
        std::string http_root_dir = check_localhost_path(argc, argv, example_name, port, index_html_file_name);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        imguiWS.init(port, http_root_dir, {"", index_html_file_name.data()});
        return imguiWS;
    }

    inline ImGuiWS &init_imgui_ws(int argc, char **argv, const std::string_view example_name, int port, const std::string_view index_html_file_name = "index.html")
    {
        static ImGuiWS imguiWS;
        std::string http_root_dir = check_localhost_path(argc, argv, example_name, port, index_html_file_name);
        imguiWS.init(port, http_root_dir, {"", index_html_file_name.data()});
        return imguiWS;
    }

    struct VSync
    {
        VSync(double rate_fps = 60.0) : tStep_us(1000000.0 / rate_fps) {}

        uint64_t tStep_us;
        uint64_t tLast_us = t_us();
        uint64_t tNext_us = tLast_us + tStep_us;

        inline uint64_t t_us() const
        {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count(); // duh ..
        }

        inline void wait()
        {
            uint64_t tNow_us = t_us();
            while (tNow_us < tNext_us - 100)
            {
                std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(0.9 * (tNext_us - tNow_us))));
                tNow_us = t_us();
            }

            tNext_us += tStep_us;
        }

        inline float delta_s()
        {
            uint64_t tNow_us = t_us();
            uint64_t res = tNow_us - tLast_us;
            tLast_us = tNow_us;
            return float(res) / 1e6f;
        }
    };
}
