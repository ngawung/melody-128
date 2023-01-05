#include <iostream>
#include <sstream>
#include <raylib.h>

#include "TestScene/TestScene.hpp"
#include "utils/Config.hpp"
#include "utils/Terminal.hpp"

namespace  {
    TerminalFont tmf;
    Terminal term;

    Color bg = {16 ,20 ,28 ,255};
    Color fg = {33 ,37 ,43 ,255};
    Color p1 = {191,189,182,255};
    Color p2 = {175,175,148,255};

    std::stringstream oss;

    void ClearStatus() {
        TerminalDrawLine(term, 0, 0, term.width, 0, {'_', bg, BLANK});
        TerminalDrawLine(term, 0, 1, term.width, 1, {' ', BLANK, fg});
        TerminalDrawLine(term, 0, 2, term.width, 2, {' ', BLANK, fg});
    }
}

UILayout::UILayout() {
    tmf = LoadTerminalFont(
        (RESOURCE_PATH + Config::FONT_FILE).c_str(),
        Config::FONT_ROW,
        Config::FONT_SIZE.x,
        Config::FONT_SIZE.y);

    term = LoadTerminal(Config::WINDOW_SIZE.x, 3, tmf);

    oss << "16,7 0x";
    oss << std::hex << (int)'a';

    BeginTextureMode(term.buffer);
    {
        ClearStatus();
        TerminalDrawText(term, 1, term.height-1, "[A] Text mode (0,0)", p2, fg);
        TerminalDrawText(term, term.width-oss.str().length()-1, term.height-1, oss.str(), p2, fg);
        for (int i=0; i<term.width; i++) {
            TerminalDrawXY(term, i, term.height-2, {'_', bg, {(unsigned char)(rand() % 256), (unsigned char)(rand() % 256), (unsigned char)(rand() % 256), 255}});
        }
    }
    EndTextureMode();
}

void UILayout::update() {
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawTextureRec(
            term.buffer.texture,
            {0.0f, 0.0f, (float)term.buffer.texture.width, -1.0f*term.buffer.texture.height},
            {0.0f, GetScreenHeight() - 3.0f*term.tmf.height}, WHITE);

        DrawFPS(GetScreenWidth() - 100, 0);
    }
    EndDrawing();

    

}