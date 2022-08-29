#pragma once
#include <string>

#include <raylib.h>

#ifdef GLOBAL_CONFIG
  #define EXTERN
#else
  #define EXTERN extern
#endif

namespace Config {
    EXTERN std::string FONT_FILE;
    EXTERN Vector2 FONT_SIZE;
    EXTERN int FONT_ROW;

    EXTERN Vector2 CANVAS_SIZE;
    EXTERN Vector2 WINDOW_SIZE;
    
    void LoadConfig(std::string file);
};

Vector2 GetTermToScreen(Vector2 pos);
Vector2 GetScreenToTerm(Vector2 pos);