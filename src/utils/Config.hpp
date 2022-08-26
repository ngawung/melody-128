#pragma once
#include <string>

#include <raylib.h>

namespace Config {
    extern std::string FONT_FILE;
    extern Vector2 FONT_SIZE;
    extern int FONT_ROW;

    extern Vector2 CANVAS_SIZE;
    extern Vector2 WINDOW_SIZE;
};

Vector2 GetTermToScreen(Vector2 pos);
Vector2 GetScreenToTerm(Vector2 pos);