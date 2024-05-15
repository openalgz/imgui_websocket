/*! \file common.h
 *  \brief Enter description here.
 */

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

namespace examples
{
#ifdef _WIN32
    inline constexpr char path_separator = '\\';
#else
    inline constexpr char path_separator = '/';
#endif

    template <typename StrT>
    concept is_string =
        std::is_same_v<std::decay_t<StrT>, std::string> || std::is_same_v<std::decay_t<StrT>, std::string_view> ||
        std::is_same_v<std::decay_t<StrT>, std::wstring> || std::is_same_v<std::decay_t<StrT>, std::wstring_view>;

    template <typename StrT_Or_PathT>
    concept is_filesystem_path_or_string =
        std::is_same_v<std::decay_t<StrT_Or_PathT>, std::filesystem::path> || is_string<StrT_Or_PathT>;

    template <is_filesystem_path_or_string StrT_Or_PathT>
    std::string truncate_directory_path_at_last_folder(const StrT_Or_PathT &path)
    {
        std::string str;

        if constexpr (std::is_same_v<std::decay_t<StrT_Or_PathT>, std::filesystem::path>)
        {
            str = path.generic_string();
        }
        else
        {
            str = path;
        }

        // On Windows folder separators may be either '/' or '\\' therefore:
        //
        size_t pos = str.find_last_of(path_separator);
        size_t alt_pos = str.find_last_of(path_separator == '\\' ? '/' : '\\');
        if (pos != std::string_view::npos && alt_pos != std::string_view::npos)
        {
            pos = std::max(pos, alt_pos);
        }
        else if (alt_pos != std::string_view::npos)
        {
            pos = alt_pos;
        }

        if (pos != std::string_view::npos)
        {
            return str.substr(0, pos);
        }
        else
        {
            return str;
        }
    }

    template <is_filesystem_path_or_string T>
    inline bool resource_exists(const T &resource_path, const std::string_view runtime_name)
    {
        namespace fs = std::filesystem;
        if (not fs::exists(resource_path))
        {
            std::cerr << "Resource path '" << resource_path << "' does not exist.\nExiting " << runtime_name << ".";
            return false;
        }
        return true;
    }

    inline ImGuiWS &start_imgui_ws(int argc, char **argv, const std::string_view example_name, int port, const std::string_view index_html_file_name = "index.html")
    {
        namespace fs = std::filesystem;

        static ImGuiWS imguiWS;

        auto http_root_dir = fs::current_path().generic_string().append("/examples");

        if (not fs::exists(http_root_dir))
        {
            http_root_dir = truncate_directory_path_at_last_folder(fs::current_path());
            std::cout << "After first truncation: " << http_root_dir << std::endl;
            http_root_dir = truncate_directory_path_at_last_folder(http_root_dir).append("/examples");
            std::cout << "After second truncation: " << http_root_dir << std::endl;
        }

        printf("Usage: %s [port] [http-root]\n", argv[0]);

        // Override the port or http root directory.
        if (argc > 1)
            port = atoi(argv[1]);
        if (argc > 2)
            http_root_dir = argv[2];

        auto resource_path = std::format("{}/{}/{}", http_root_dir, example_name, index_html_file_name);

        if (not fs::exists(resource_path))
        {
            std::cerr << std::format("Resource, '{}', could not be found!\nExiting '{}' startup.\n", resource_path, example_name);
            exit(0);
        }

        std::cout << "\nurl:localhost:" << port << std::endl;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        imguiWS.init(port, http_root_dir + std::string("/") + std::string(example_name.data()), {"", index_html_file_name.data()});

        return imguiWS;
    }

    inline ImGuiWS &init_imgui_ws(int argc, char **argv, const std::string_view example_name, int port, const std::string_view index_html_file_name = "index.html")
    {
        namespace fs = std::filesystem;

        auto http_root_dir = fs::current_path().generic_string().append("/examples");

        if (not fs::exists(http_root_dir))
        {
            http_root_dir = truncate_directory_path_at_last_folder(fs::current_path());
            std::cout << "After first truncation: " << http_root_dir << std::endl;
            http_root_dir = truncate_directory_path_at_last_folder(http_root_dir).append("/examples");
            std::cout << "After second truncation: " << http_root_dir << std::endl;
        }

        static ImGuiWS imguiWS;

        printf("Usage: %s [port] [http-root]\n", argv[0]);

        // Override the port or http root directory.
        if (argc > 1)
            port = atoi(argv[1]);
        if (argc > 2)
            http_root_dir = argv[2];

        auto resource_path = std::format("{}/{}/{}", http_root_dir, example_name, index_html_file_name);

        if (not fs::exists(resource_path))
        {
            std::cerr << std::format("Resource, '{}', could not be found!\nExiting '{}' startup.\n", resource_path, example_name);
            exit(0);
        }

        std::cout << "\nurl: localhost:" << port << std::endl;

        imguiWS.init(port, http_root_dir + std::string("/") + std::string(example_name.data()), {"", index_html_file_name.data()});

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