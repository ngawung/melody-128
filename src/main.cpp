#define RAYGUI_IMPLEMENTATION
#define RAYMATH_IMPLEMENTATION

#include <raylib.h>
#include <rlImGui.h>
#include <raymath.h>
#include <raygui.h>

#include "core/App.hpp"
#include "utils/Config.hpp"
#include "scene/MainEditor.hpp"

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void mainLoop(void);

int main()
{

    Config::LoadConfig(RESOURCE_PATH"config.ini");

    InitWindow(960, 600, "Melody-128");

    // SetTargetFPS(60);
    SetExitKey(-1);
    SetWindowState(
        FLAG_VSYNC_HINT |
        FLAG_WINDOW_RESIZABLE |
        FLAG_MSAA_4X_HINT);

    rlImGuiSetup(true);
    GuiLoadStyle(RESOURCE_PATH"default.rgs");

    App::getInstance().setScene(std::make_shared<MainEditor>());

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(mainLoop, 0, 1);
#else
    SetTargetFPS(60);
    mainLoop();
#endif

    rlImGuiShutdown();
    CloseWindow();
    return 0;
}

void mainLoop(void)
{
    App &instance = App::getInstance();
    while(!WindowShouldClose()) {
        instance.update();
        if (IsKeyReleased(KEY_F11)) ToggleFullscreen();
    }
}

Vector2 GetTermToScreen(Vector2 pos) {
    return {pos.x*Config::FONT_SIZE.x, pos.y*Config::FONT_SIZE.y};
}

Vector2 GetScreenToTerm(Vector2 pos) {
    return {std::floor(pos.x/Config::FONT_SIZE.x), std::floor(pos.y/Config::FONT_SIZE.y)};
}
