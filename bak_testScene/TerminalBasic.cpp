#include <raylib.h>
#include <iostream>
#include <cmath>
#include <sstream>

#include "TestScene/TestScene.hpp"
#include "utils/Config.hpp"
#include "utils/Terminal.hpp"

namespace {
    TerminalFont tmf;
    Terminal term;

    Vector2 start = {0.0f, 0.0f};
    Vector2 end = {0.0f, 0.0f};
    Vector2 cursor = {0.0f, 0.0f};
    int indexLine = 0;
    
    float pressCounter = 0;
    float pressDelay = 0;
    float colorAlpha = 0.0f;
}

TerminalBasic::TerminalBasic() {
    tmf = LoadTerminalFont((RESOURCE_PATH + Config::FONT_FILE).c_str(), Config::FONT_ROW, Config::FONT_SIZE.x, Config::FONT_SIZE.y);
    term = LoadTerminal(Config::CANVAS_SIZE.x, Config::CANVAS_SIZE.y, tmf);
    
    TerminalClear(term, {0xfa, GRAY, BLACK});
}

void TerminalBasic::update() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // terminal buffer
    DrawTextureRec(
        term.buffer.texture,
        {0.0f, 0.0f, (float)term.buffer.texture.width, -1.0f*term.buffer.texture.height},
        {0.0f, 0.0f}, WHITE);

    // cursor
    DrawRectangleLines((int)cursor.x*Config::FONT_SIZE.x, (int)cursor.y*Config::FONT_SIZE.y, Config::FONT_SIZE.x, Config::FONT_SIZE.y, ORANGE);
    DrawLine(indexLine*Config::FONT_SIZE.x, 0, indexLine*Config::FONT_SIZE.x, GetScreenWidth(), ColorAlpha(ORANGE, colorAlpha));
    if (colorAlpha > 0) colorAlpha -= GetFrameTime()*2;
    if (colorAlpha < 0) colorAlpha = 0.0f;

    std::stringstream oss;
    oss << pressCounter << "\n"
        << GetKeyPressed() << "\n";
    DrawText(oss.str().c_str(), 5, 5, 12, WHITE);

    DrawFPS(GetScreenWidth() - 100, 0);
    EndDrawing();

    cursorInput(GetKeyPressed());
    characterInput(GetCharPressed());
}

void TerminalBasic::cursorInput(int keycode) {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        cursor = GetScreenToTerm(GetMousePosition());
        indexLine = cursor.x;
        colorAlpha = 1.0f;
    }

    right_press = IsKeyDown(KEY_RIGHT);
    left_press = IsKeyDown(KEY_LEFT);
    up_press = IsKeyDown(KEY_UP);
    down_press = IsKeyDown(KEY_DOWN);
    back_press = IsKeyDown(KEY_BACKSPACE);

    // held
    if (right_press || left_press || up_press || down_press || back_press) {
        pressCounter += GetFrameTime();
        if (pressCounter >= 0.3) pressDelay += GetFrameTime();

        if (pressDelay >= 0.05) {
            pressDelay = 0;
            if (right_press) moveCursor(1.0f, 0);
            if (left_press) moveCursor(-1.0f, 0);
            if (up_press) moveCursor(0, -1.0f);
            if (down_press) moveCursor(0, 1.0f);
            if (back_press) {
                TerminalDrawXY(term, cursor, {' ', WHITE, BLACK});
                moveCursor(-1, 0);
            }
        }
    }

    // release
    if (IsKeyReleased(KEY_RIGHT) || 
        IsKeyReleased(KEY_LEFT) || 
        IsKeyReleased(KEY_UP) || 
        IsKeyReleased(KEY_DOWN) ||
        IsKeyReleased(KEY_BACKSPACE))
        pressCounter = pressDelay = 0;

    // reset
    right_press = left_press = up_press = down_press = back_press = false;

    // down
    if (keycode == 0) return;
    switch(keycode) {
        case KEY_LEFT:
        moveCursor(-1, 0);
        break;

        case KEY_RIGHT:
        moveCursor(1, 0);
        break;

        case KEY_UP:
        moveCursor(0, -1);
        break;

        case KEY_DOWN:
        moveCursor(0, 1);
        break;

        case KEY_ENTER:
        moveCursor(0, 1.0f);
        cursor.x = indexLine;
        colorAlpha = 1.0f;
        break;

        case KEY_DELETE:
        TerminalClear(term, {0xfa, GRAY, BLACK});
        break;

        case KEY_BACKSPACE:
        TerminalDrawXY(term, cursor, {' ', WHITE, BLACK});
        moveCursor(-1, 0);
        break;

        case KEY_F1:
        Image img = LoadImageFromTexture(term.buffer.texture);
        ImageFlipVertical(&img);
        ExportImage(img, "./export.png");
        UnloadImage(img);
        break;
    }
}

void TerminalBasic::characterInput(int unicode) {
    if (unicode == 0) return;
    
    TerminalDrawXY(term, cursor, {(uint8_t)unicode, WHITE, BLACK});
    moveCursor(1, 0);
    colorAlpha = 1.0f;
}

void TerminalBasic::moveCursor(int x, int y) {
    cursor.x += x;
    cursor.y += y;

    if (cursor.x > term.width-1) {
        cursor.y++;
        cursor.x = indexLine;
    } 

    else if (cursor.x < 0) {
        cursor.y--;
        cursor.x = term.width-1;
    }

    if (cursor.y > term.height-1) {
        cursor.y = 0;
    }

    else if (cursor.y < 0) {
        cursor.y = term.height-1;
    }
}