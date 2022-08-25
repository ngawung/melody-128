#pragma once

#include <cstdint>
#include <memory>
#include <raylib.h>

struct TerminalFont {
    uint16_t row;
    Texture2D texture;
};

struct Glyph {
    uint8_t symbol;
    Color foreground;
    Color background;

    bool compare(Glyph &g) {
        return (g.symbol == symbol &&
                g.foreground.r == foreground.r &&
                g.foreground.g == foreground.g &&
                g.foreground.b == foreground.b &&
                g.foreground.a == foreground.a &&
                g.background.r == background.r &&
                g.background.g == background.g &&
                g.background.b == background.b &&
                g.background.a == background.a);
    }
};

struct Terminal {
    TerminalFont tmf;
    uint16_t width;
    uint16_t height;
    std::unique_ptr<Glyph[]> chars;
    RenderTexture buffer;
};

TerminalFont LoadTerminalFont(const char * file, uint16_t row);
void UnloadTerminalFont(const TerminalFont &tmf);

Terminal LoadTerminal(uint16_t width, uint16_t height, TerminalFont tmf);
void UnloadTerminal(const Terminal &term);

Glyph *GetTerminalXY(const Terminal &term, int x, int y);

void ValidateTerminal(const Terminal &term);
void TerminalClear(const Terminal &term, Glyph g);

void TerminalDrawXY(const Terminal &term, int x, int y, Glyph g);
void TerminalDrawXY(const Terminal &term, Vector2 pos, Glyph g);

void TerminalDrawLine(const Terminal &term, int x1, int y1, int x2, int y2, Glyph g);
void TerminalDrawLine(const Terminal &term, Vector2 start, Vector2 end, Glyph g);

void TerminalDrawCircle(const Terminal &term, int xc, int yc, int r, Glyph g);
void TerminalDrawCircle(const Terminal &term, Vector2 pos, int r, Glyph g);

void TerminalDrawRect(const Terminal &term, int x, int y, int w, int h, Glyph g);
void TerminalDrawRect(const Terminal &term, Rectangle rect, Glyph g);

void TerminalFill4(const Terminal &term, int x, int y, Glyph g, Glyph boundary);
void TerminalFill4(const Terminal &term, int x, int y, Glyph g);
void TerminalFill4(const Terminal &term, Vector2 pos, Glyph g);