#include <algorithm>
#include <cmath>
#include <iostream>

#include "utils/Terminal.hpp"

/* Function that returns -1,0,1 depending on whether x */
/* is <0, =0, >0 respectively */
#define sign(x) ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define x_off x - cx
#define y_off y - cy

TerminalFont LoadTerminalFont(const char * file, int row, int width, int height) {
    Image img = LoadImage(file);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color *color = LoadImageColors(img);
    ImageColorReplace(&img, color[0], ColorAlpha(BLACK, 0));
    
    TerminalFont tmf = {row, width, height, LoadTextureFromImage(img)};

    UnloadImageColors(color);
    UnloadImage(img);

    return tmf;
}

void UnloadTerminalFont(const TerminalFont &tmf) {
    UnloadTexture(tmf.texture);
}

Terminal LoadTerminal(int width, int height, TerminalFont tmf) {
    return {
        tmf, width, height,
        std::make_unique<Glyph[]>(width*height*sizeof(Glyph)),
        LoadRenderTexture(width*tmf.width, height*tmf.height)
    };
}

void UnloadTerminal(const Terminal &term) {
    // MemFree(term.chars);
    UnloadRenderTexture(term.buffer);
}

void TerminalRedraw(const Terminal &term) {
    BeginTextureMode(term.buffer);

    for (int y=0; y<term.height; y++) {
        for (int x=0; x<term.width; x++) {

            Vector2 pos = {(float)x*term.tmf.width, (float)y*term.tmf.height};
            Glyph g = term.chars[term.width*y + x];

            DrawRectangle(pos.x, pos.y, term.tmf.width, term.tmf.height, g.background);

            if (g.symbol != ' ') {
                DrawTextureRec(
                    term.tmf.texture,
                    {
                        (float)(g.symbol % 16)*term.tmf.width,
                        (float)(g.symbol / 16)*term.tmf.height,
                        (float)term.tmf.width, (float)term.tmf.height
                    },
                    pos, g.foreground);
            }

        }
    }

    EndTextureMode();
}

void TerminalClear(const Terminal &term, Glyph g) {
    for(int i=0; i<term.width*term.height; i++) {
        term.chars[i] = g;
    }
    TerminalRedraw(term);
}

//  TODO: throw exception here
Glyph *GetTerminalXY(const Terminal &term, int x, int y) {
    if (x >= 0 && x < term.width && y >= 0 && y < term.height)
        return &term.chars[(int)(term.width*y + x)];
    else
        return nullptr;
}

void TerminalDrawXY(const Terminal &term, int x, int y, Glyph g) {
    if (x >= 0 && x < term.width && y >= 0 && y < term.height) {
        term.chars[(int)(term.width*y + x)] = g;

        // BeginTextureMode(term.buffer);
        Vector2 pos = {(float)x*term.tmf.width, (float)y*term.tmf.height};
        
        DrawRectangle(pos.x, pos.y, term.tmf.width, term.tmf.height, g.background);

        if (g.symbol != ' ') {
            DrawTextureRec(
                term.tmf.texture,
                {
                    (float)(g.symbol % 16)*term.tmf.width,
                    (float)(g.symbol / 16)*term.tmf.height,
                    (float)term.tmf.width, (float)term.tmf.height
                },
                pos, g.foreground);
        }
        // EndTextureMode();
    }
}

void TerminalDrawXY(const Terminal &term, Vector2 pos, Glyph g) {
    TerminalDrawXY(term, pos.x, pos.y, g);
}

void TerminalDrawLine(const Terminal &term, int x1, int y1, int x2, int y2, Glyph g) {
    int Dx = x2 - x1;
    int Dy = y2 - y1;

    //# Increments
    int Sx = sign(Dx); 
    int Sy = sign(Dy);

    //# Segment length
    Dx = abs(Dx); 
    Dy = abs(Dy); 
    int D = std::max(Dx, Dy);

    //# Initial remainder
    double R = D / 2;

    int X = x1;
    int Y = y1;
    if(Dx > Dy)
    {   
        //# Main loop
        for(int I=0; I<D; I++)
        {   
            TerminalDrawXY(term, X, Y, g); 
            //# Update (X, Y) and R
            X+= Sx; R+= Dy; //# Lateral move
            if (R >= Dx)
            {
                Y+= Sy; 
                R-= Dx; //# Diagonal move
            }
        }
    }
    else
    {   
        //# Main loop
        for(int I=0; I<D; I++)
        {   
            TerminalDrawXY(term, X, Y, g);
            // Set(X, Y, color);
            //# Update (X, Y) and R
            Y+= Sy; 
            R+= Dx; //# Lateral move
            if(R >= Dy)
            {    
                X+= Sx; 
                R-= Dy; //# Diagonal move
            }
        }
    }
}

void TerminalDrawLine(const Terminal &term, Vector2 start, Vector2 end, Glyph g) {
    TerminalDrawLine(term, start.x, start.y, end.x, end.y, g);
}


void TerminalDrawCircle(const Terminal &term, int xc, int yc, int r, Glyph g) {
    Vector2 last_pos;

    for (int i=0; i<=360; i+=10) {
        Vector2 pos = {(float)std::sin(i*DEG2RAD)*r + xc*term.tmf.width, (float)std::cos(i*DEG2RAD)*r + yc*term.tmf.height};

        if (i==0) {
            last_pos = pos;
            continue;
        }

        TerminalDrawLine(term, last_pos.x/term.tmf.width, last_pos.y/term.tmf.height, pos.x/term.tmf.width, pos.y/term.tmf.height, g);
        last_pos = pos;
    }
}

void TerminalDrawCircle(const Terminal &term, Vector2 pos, int r, Glyph g) {
    TerminalDrawCircle(term, pos.x, pos.y, r, g);
}

void TerminalDrawRect(const Terminal &term, int x, int y, int w, int h, Glyph g) {
    TerminalDrawLine(term, x, y, x+w+1, y, g);
    TerminalDrawLine(term, x, y, x, y+h+1, g);
    TerminalDrawLine(term, x+w, y+h, x+w, y, g);
    TerminalDrawLine(term, x+w, y+h, x, y+h, g);
}

void TerminalDrawRect(const Terminal &term, Rectangle rect, Glyph g) {
    TerminalDrawRect(term, rect.x, rect.y, rect.width, rect.height, g);
}

void TerminalDrawText(const Terminal &term, int x, int y, std::string str, Color fg, Color bg) {
    int i = 0;
    for(const unsigned char &c : str) {
        TerminalDrawXY(term, x+i, y, {c, fg, bg});
        i++;
    }
}

void TerminalDrawText(const Terminal &term, Vector2 pos, std::string str, Color fg, Color bg) {
    TerminalDrawText(term, pos.x, pos.y, str, fg, bg);
}

void TerminalFill4(const Terminal &term, int x, int y, Glyph g, Glyph boundary) {
    if (x >= 0 && x < term.width && y >= 0 && y < term.height) {
        if(GetTerminalXY(term, x, y)->compare(boundary) &&
           !GetTerminalXY(term, x, y)->compare(g))
        {
            TerminalDrawXY(term, x, y, g);
            TerminalFill4(term, x + 1, y, g, boundary);
            TerminalFill4(term, x, y + 1, g, boundary);
            TerminalFill4(term, x - 1, y, g, boundary);
            TerminalFill4(term, x, y - 1, g, boundary);
        }
    }
}

void TerminalFill4(const Terminal &term, int x, int y, Glyph g) {
    TerminalFill4(term, x, y, g, *GetTerminalXY(term, x, y));
}

void TerminalFill4(const Terminal &term, Vector2 pos, Glyph g) {
    TerminalFill4(term, pos.x, pos.y, g);
}