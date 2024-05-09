/*! \file common.h
 *  \brief Enter description here.
 */

#pragma once

#include <chrono>
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

ImGuiWS& start_imgui_ws(int argc, char ** argv, const char* example_name, int port = 3000, const char* http_root = "../../examples", const char* index_html = "index.html")
{
    namespace fs = std::filesystem;

    printf("Usage: %s [port] [http-root]\n", argv[0]);

    std::string httpRoot = http_root;

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) httpRoot = argv[2];

    std::string rp(http_root);
    rp.append(example_name).append("/").append(index_html);

    // Check for resource:
    fs::path resource_path(rp);
    resource_path = fs::absolute(resource_path);
    if (not fs::exists(resource_path)){

        std::cerr
                << "Resource, '"
                << resource_path.string()
                << "' could not be found!\nExiting '"
                << example_name
                << "' startup!";

        exit(0);
    }

    std::cout << "localhost:" << port;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    static ImGuiWS imguiWS;
    imguiWS.init(port, httpRoot + example_name, { "", index_html });

    return imguiWS;
}

ImGuiWS& init_imgui_ws(int argc, char ** argv, const char* example_name, int port = 3000, const char* http_root = "../../examples", const char* index_html = "index.html")
{
    namespace fs = std::filesystem;

    printf("Usage: %s [port] [http-root]\n", argv[0]);

    std::string httpRoot = http_root;

    if (argc > 1) port = atoi(argv[1]);
    if (argc > 2) httpRoot = argv[2];

    std::string rp(http_root);
    rp.append(example_name).append("/").append(index_html);

    // Check for resource:
    fs::path resource_path(rp);
    resource_path = fs::absolute(resource_path);
    if (not fs::exists(resource_path)){

        std::cerr
                << "Resource, '"
                << resource_path.string()
                << "' could not be found!\nExiting '"
                << example_name
                << "' startup!";

        exit(0);
    }

    std::cout << "localhost:" << port;

    static ImGuiWS imguiWS;
    imguiWS.init(port, httpRoot + example_name, { "", index_html });

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
