#include <iostream>
#include <raylib.h>
#include <melody.hpp>

#define RAYGUI_IMPLEMENTATION
#define RAYMATH_IMPLEMENTATION

#include <raymath.h>
#include <raygui.h>
#include <rlImGui.h>

#include "utils/Config.hpp"
#include "TestScene/TestScene.hpp"

int main() {

    // SetTargetFPS(60);
    SetExitKey(0);
    InitWindow(
        Config::FONT_SIZE.x * Config::WINDOW_SIZE.x, 
        Config::FONT_SIZE.y * Config::WINDOW_SIZE.y,
        "Melody-128");

    rlImGuiSetup(true);
    melody::App MainEngine;
    MainEngine.start(new CamNavigation());

    while(!WindowShouldClose()) {
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