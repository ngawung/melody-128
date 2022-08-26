#include <iostream>
#include <raylib.h>

#include "TestScene/TestScene.hpp"


namespace  {

}

ConfigLoader::ConfigLoader() {
    

}

void ConfigLoader::update() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawFPS(GetScreenWidth() - 100, 0);
    EndDrawing();
}