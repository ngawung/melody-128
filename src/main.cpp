#include <iostream>
#include <raylib.h>
#include <melody.hpp>

#include "Global.hpp"
#include "TestScene/TestScene.hpp"

int main() {

    // SetTargetFPS(60);
    SetExitKey(0);
    InitWindow(
        FONT_SIZE.x * SCREEN_SIZE.x, 
        FONT_SIZE.y * SCREEN_SIZE.y,
        "Melody-128");

    melody::App MainEngine;
    MainEngine.start(new TerminalBasic());

    while(!WindowShouldClose()) {
        MainEngine.update();
        if (IsKeyReleased(KEY_F11)) ToggleFullscreen();
    }

    return 0;
}

Vector2 GetTermToScreen(Vector2 pos) {
    return {pos.x*FONT_SIZE.x, pos.y*FONT_SIZE.y};
}

Vector2 GetScreenToTerm(Vector2 pos) {
    return {std::floor(pos.x/FONT_SIZE.x), std::floor(pos.y/FONT_SIZE.y)};
}