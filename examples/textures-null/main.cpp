#include "examples-common.h"

using namespace examples;

// texture IDs - these are user generated. Use whatever fits you
uint32_t g_texture0Id = 100;
uint32_t g_texture1Id = 200;
uint32_t g_texture2Id = 300;

void regenerate(ImGuiWS & imguiWS, int time) {
    {
        // texture sizes must be powers of 2
        int width = 16;
        int height = 16;
        std::vector<char> texture0(width*height);
        for (int i = 0; i < width*height; ++i) {
            texture0[i] = rand()%256;
        }
        imguiWS.setTexture(g_texture0Id, ImGuiWS::Texture::Type::Gray8, width, height, (const char *) texture0.data());
    }

    {
        int width = 32;
        int height = 32;
        std::vector<char> texture0(3*width*height);
        for (int i = 0; i < width*height; ++i) {
            texture0[3*i + 0] = (i*10 + time)%255;
            texture0[3*i + 1] = (i*13 + time)%255;
            texture0[3*i + 2] = (i*19 + time)%255;
        }
        imguiWS.setTexture(g_texture1Id, ImGuiWS::Texture::Type::RGB24, width, height, (const char *) texture0.data());
    }

    {
        int width = 64;
        int height = 64;
        std::vector<char> texture0(4*width*height);
        for (int i = 0; i < width*height; ++i) {
            texture0[4*i + 0] = rand()%256;
            texture0[4*i + 1] = rand()%256;
            texture0[4*i + 2] = rand()%256;
            texture0[4*i + 3] = (i*19 + time)%255;
        }
        imguiWS.setTexture(g_texture2Id, ImGuiWS::Texture::Type::RGBA32, width, height, (const char *) texture0.data());
    }
}

int main(int argc, char ** argv) {

    auto& imguiWS = start_imgui_ws(argc, argv, "textures-null", 3003);
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    // prepare font texture
    {
        unsigned char * pixels;
        int width, height;
        ImGui::GetIO().Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
        imguiWS.setTexture(0, ImGuiWS::Texture::Type::Alpha8, width, height, (const char *) pixels);
    }

    // generate some random textures
    regenerate(imguiWS, 0);

    VSync vsync;

    while (true) {
        // regenerate random textures every ~5 seconds
        {
            static int counter = 0;
            if (++counter % (5*60) == 0) {
                regenerate(imguiWS, counter);
            }
        }

        io.DisplaySize = ImVec2(640, 480);
        io.DeltaTime = vsync.delta_s();

        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 20, 20 });
        ImGui::SetNextWindowSize({ 400, 100 });
        ImGui::Begin("Hello, world!");
        ImGui::Text("Connected clients: %d", imguiWS.nConnected());
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::SetNextWindowPos({ 20, 140 });
        ImGui::SetNextWindowSize({ 420, 200 });
        ImGui::Begin("Some random images");
        ImGui::Text("The images are regenerated on the server every ~5 seconds");
        ImGui::Image((void *)(intptr_t) g_texture0Id, { 128, 128 }, ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
        ImGui::SameLine();
        ImGui::Image((void *)(intptr_t) g_texture1Id, { 128, 128 }, ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
        ImGui::SameLine();
        ImGui::Image((void *)(intptr_t) g_texture2Id, { 128, 128 }, ImVec2(0,0), ImVec2(1,1), ImVec4(1.0f,1.0f,1.0f,1.0f), ImVec4(1.0f,1.0f,1.0f,0.5f));
        ImGui::End();

        ImGui::SetNextWindowPos({ 20, 360 });
        ImGui::ShowAboutWindow();

        // generate ImDrawData
        ImGui::Render();

        // store ImDrawData for asynchronous dispatching to WS clients
        imguiWS.setDrawData(ImGui::GetDrawData());

        // if not clients are connected, just sleep to save CPU
        do {
            vsync.wait();
        } while (imguiWS.nConnected() == 0);
    }

    ImGui::DestroyContext();

    return 0;
}
