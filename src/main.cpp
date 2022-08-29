#define RAYGUI_IMPLEMENTATION
#define RAYMATH_IMPLEMENTATION

#include <iostream>
#include <raylib.h>
#include <melody.hpp>
#include <rlImGui.h>

#include <raymath.h>
#include <raygui.h>

#include "utils/Config.hpp"
#include "scene/MainEditor.hpp"
#include "TestScene/TestScene.hpp"

int main() {

    Config::LoadConfig(RESOURCE_PATH"config.ini");

    InitWindow(
        Config::FONT_SIZE.x * Config::WINDOW_SIZE.x, 
        Config::FONT_SIZE.y * Config::WINDOW_SIZE.y,
        "Melody-128");
    
    // SetTargetFPS(60);
    SetExitKey(-1);
    SetWindowState(
        FLAG_VSYNC_HINT |
        FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT);

    rlImGuiSetup(true);
    GuiLoadStyle(RESOURCE_PATH"default.rgs");

    melody::App MainEngine;
    MainEngine.start(new MainEditor());

    while(MainEngine.Running) {
        MainEngine.update();
        if (IsKeyReleased(KEY_F11)) ToggleFullscreen();
    }

    rlImGuiShutdown();
    return 0;
}

Vector2 GetTermToScreen(Vector2 pos) {
    return {pos.x*Config::FONT_SIZE.x, pos.y*Config::FONT_SIZE.y};
}

Vector2 GetScreenToTerm(Vector2 pos) {
    return {std::floor(pos.x/Config::FONT_SIZE.x), std::floor(pos.y/Config::FONT_SIZE.y)};
}