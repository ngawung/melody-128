#include <raylib.h>
#include <rlgl.h>
#include <GLFW/glfw3.h>
#include <fmt/core.h>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "TestScene/TestScene.hpp"
#include "utils/Config.hpp"
#include "utils/Terminal.hpp"

namespace {
    TerminalFont tmf;
    Texture2D tex;
    GLFWwindow *glfwWindow;
}

EpicGLFW::EpicGLFW() {
    tex = LoadTexture(RESOURCE_PATH"Andux_cp866ish.png");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    glfwWindow = glfwGetCurrentContext();
    ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // tmf = LoadTerminalFont(
    //     (RESOURCE_PATH + Config::FONT_FILE).c_str(),
    //     Config::FONT_ROW,
    //     Config::FONT_SIZE.x,
    //     Config::FONT_SIZE.y);
}

void EpicGLFW::update() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    rlDrawRenderBatchActive();

    ClearBackground(RAYWHITE);

    static bool open = true;
    ImGui::ShowDemoWindow(&open);


    int texId = (tex.id + 1);
    ImGui::Begin("Test");
    ImGui::Image(&tex.id , ImVec2(tex.width, tex.height));
    // for (int i = 0; i < 256; i++)
    // {
    //     if ((i % 16) != 0) ImGui::SameLine();
    //     ImGui::Button(fmt::format("{}", i).c_str(), {10.0f, 20.0f});
    // }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    // glfwSwapBuffers(glfwWindow);

    DrawFPS(0, 0);
    EndDrawing();
}

EpicGLFW::~EpicGLFW() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}