#include <raylib.h>
#include <iostream>
#include <cmath>

#include "TestScene/TestScene.hpp"
#include "utils/Terminal.hpp"
#include "Global.hpp"

namespace {
    TerminalFont tmf;
    Terminal term;

    Vector2 start = {0.0f, 0.0f};
    Vector2 end = {0.0f, 0.0f};
}

TerminalBasic::TerminalBasic() {
    tmf = LoadTerminalFont(RESOURCE_PATH"VGA9x16.png", 16);
    term = LoadTerminal(SCREEN_SIZE.x, SCREEN_SIZE.y, tmf);
    
    TerminalClear(term, {'.', GRAY, BLACK});
    TerminalDrawXY(term, 10, 10, {'(', WHITE, BLACK});
    TerminalDrawXY(term, 11, 10, {'_', WHITE, BLACK});
    TerminalDrawXY(term, 11, 9, {'_', WHITE, BLACK});
    TerminalDrawXY(term, 12, 10, {')', WHITE, BLACK});
    ValidateTerminal(term);

    std::cout << sizeof(Glyph) << "\n";


    std::unique_ptr<Glyph> test {new Glyph[term.width]};
    test.release();
}

void TerminalBasic::update() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    DrawTextureRec(
        term.buffer.texture,
        {0.0f, 0.0f, (float)term.buffer.texture.width, -1.0f*term.buffer.texture.height},
        {0.0f, 0.0f}, WHITE);

    DrawFPS(GetScreenWidth() - 100, 0);
    EndDrawing();

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        start = GetScreenToTerm(GetMousePosition());
        TerminalClear(term, {'K', BLACK, RED});
        TerminalDrawLine(term, start, end, {0x01, WHITE, BLACK});
        ValidateTerminal(term);
    }

    else if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON)) {
        // end = GetScreenToTerm(GetMousePosition());
        TerminalClear(term, {'.', GRAY, BLACK});
        // TerminalDrawLine(term, start, end, {0x01, WHITE, BLACK});
        ValidateTerminal(term);
    }

    if (IsMouseButtonReleased(MOUSE_MIDDLE_BUTTON)) {
        // TerminalClear(term, {'K', BLACK, RED});
        
        Vector2 pos = GetScreenToTerm(GetMousePosition());
        TerminalDrawCircle(term, pos, 70, {0x01, WHITE, BLACK});
        ValidateTerminal(term);
    }

    if (IsKeyReleased(KEY_SPACE)) {
        // TerminalClear(term, {'K', BLACK, RED});

        Vector2 pos = GetScreenToTerm(GetMousePosition());
        TerminalDrawRect(term, {pos.x - 8, pos.y - 5, 16, 10}, {0x01, WHITE, BLACK});
        ValidateTerminal(term);
    }

    if (IsKeyReleased(KEY_ENTER)) {
        // TerminalClear(term, {'K', BLACK, RED});

        Vector2 pos = GetScreenToTerm(GetMousePosition());
        TerminalFill4(term, pos.x, pos.y, {'A', WHITE, BLACK});
        ValidateTerminal(term);
    }
}