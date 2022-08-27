#include <iostream>
#include <sstream>
#include <memory>

#include <raylib.h>
#include <raymath.h>
#include <raygui.h>
#include <imgui.h>
#include <rlImGui.h>

#include "TestScene/TestScene.hpp"
#include "utils/Config.hpp"
#include "utils/Terminal.hpp"

namespace {
    Color bg    = {16 ,20 ,28 ,255};
    Color panel = {33 ,37 ,43 ,255};
    Color p1    = {191,189,182,255};
    Color p2    = {175,175,148,255};

    std::unique_ptr<Color[]> palette;
    Color currentFg = p1;
    Color currentBg = bg;
    Vector3 currentHSV;
    float lastHue = 0.0f;

    bool enableFPS = true;

    TerminalFont tmf;
    Terminal term;
    Terminal status;
    Terminal overlay;

    std::stringstream statusLeft;
    std::stringstream statusRight;
    std::string statusLog;

    Camera2D cam;
    float lastScrollWheel;
    Vector2 dragStartPos;
    Vector2 dragEndPos;

    Vector2 cursor = {0.0f, 0.0f};
    Vector2 cursorGhost = {0.0f, 0.0f};
    Vector2 cursorGhostTemp = {0.0f, 0.0f};
    int cursorIndex = 0;

    float pressCounter = 0;
    float pressDelay = 0;
    float colorAlpha = 0.0f;

    bool redrawStatus = true;

    bool isMouseInGrid() {
        return (GetMouseY() < (GetScreenHeight() - status.buffer.texture.height));
    }

    bool isMouseInPanel() {
        return (GetMouseY() > (GetScreenHeight() - status.buffer.texture.height));
    }

    // Convert color data from RGB to HSV
    // NOTE: Color data should be passed normalized
    Vector3 ConvertRGBtoHSV(Vector3 rgb)
    {
        Vector3 hsv = { 0 };
        float min = 0.0f;
        float max = 0.0f;
        float delta = 0.0f;

        min = (rgb.x < rgb.y)? rgb.x : rgb.y;
        min = (min < rgb.z)? min  : rgb.z;

        max = (rgb.x > rgb.y)? rgb.x : rgb.y;
        max = (max > rgb.z)? max  : rgb.z;

        hsv.z = max;            // Value
        delta = max - min;

        if (delta < 0.00001f)
        {
            hsv.y = 0.0f;
            hsv.x = 0.0f;           // Undefined, maybe NAN?
            return hsv;
        }

        if (max > 0.0f)
        {
            // NOTE: If max is 0, this divide would cause a crash
            hsv.y = (delta/max);    // Saturation
        }
        else
        {
            // NOTE: If max is 0, then r = g = b = 0, s = 0, h is undefined
            hsv.y = 0.0f;
            hsv.x = 0.0f;           // Undefined, maybe NAN?
            return hsv;
        }

        // NOTE: Comparing float values could not work properly
        if (rgb.x >= max) hsv.x = (rgb.y - rgb.z)/delta;    // Between yellow & magenta
        else
        {
            if (rgb.y >= max) hsv.x = 2.0f + (rgb.z - rgb.x)/delta;  // Between cyan & yellow
            else hsv.x = 4.0f + (rgb.x - rgb.y)/delta;      // Between magenta & cyan
        }

        hsv.x *= 60.0f;     // Convert to degrees

        if (hsv.x < 0.0f) hsv.x += 360.0f;

        return hsv;
    }

    // Convert color data from HSV to RGB
    // NOTE: Color data should be passed normalized
    Vector3 ConvertHSVtoRGB(Vector3 hsv)
    {
        Vector3 rgb = { 0 };
        float hh = 0.0f, p = 0.0f, q = 0.0f, t = 0.0f, ff = 0.0f;
        long i = 0;

        // NOTE: Comparing float values could not work properly
        if (hsv.y <= 0.0f)
        {
            rgb.x = hsv.z;
            rgb.y = hsv.z;
            rgb.z = hsv.z;
            return rgb;
        }

        hh = hsv.x;
        if (hh >= 360.0f) hh = 0.0f;
        hh /= 60.0f;

        i = (long)hh;
        ff = hh - i;
        p = hsv.z*(1.0f - hsv.y);
        q = hsv.z*(1.0f - (hsv.y*ff));
        t = hsv.z*(1.0f - (hsv.y*(1.0f - ff)));

        switch (i)
        {
            case 0:
            {
                rgb.x = hsv.z;
                rgb.y = t;
                rgb.z = p;
            } break;
            case 1:
            {
                rgb.x = q;
                rgb.y = hsv.z;
                rgb.z = p;
            } break;
            case 2:
            {
                rgb.x = p;
                rgb.y = hsv.z;
                rgb.z = t;
            } break;
            case 3:
            {
                rgb.x = p;
                rgb.y = q;
                rgb.z = hsv.z;
            } break;
            case 4:
            {
                rgb.x = t;
                rgb.y = p;
                rgb.z = hsv.z;
            } break;
            case 5:
            default:
            {
                rgb.x = hsv.z;
                rgb.y = p;
                rgb.z = q;
            } break;
        }

        return rgb;
    }
    void HSVtoRGB(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) {
      float fC = fV * fS; // Chroma
      float fHPrime = fmod(fH / 60.0, 6);
      float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
      float fM = fV - fC;
      
      if(0 <= fHPrime && fHPrime < 1) {
        fR = fC;
        fG = fX;
        fB = 0;
      } else if(1 <= fHPrime && fHPrime < 2) {
        fR = fX;
        fG = fC;
        fB = 0;
      } else if(2 <= fHPrime && fHPrime < 3) {
        fR = 0;
        fG = fC;
        fB = fX;
      } else if(3 <= fHPrime && fHPrime < 4) {
        fR = 0;
        fG = fX;
        fB = fC;
      } else if(4 <= fHPrime && fHPrime < 5) {
        fR = fX;
        fG = 0;
        fB = fC;
      } else if(5 <= fHPrime && fHPrime < 6) {
        fR = fC;
        fG = 0;
        fB = fX;
      } else {
        fR = 0;
        fG = 0;
        fB = 0;
      }
      
      fR += fM;
      fG += fM;
      fB += fM;
    }
}

CamNavigation::CamNavigation() {
    tmf = LoadTerminalFont(
        (RESOURCE_PATH + Config::FONT_FILE).c_str(),
        Config::FONT_ROW,
        Config::FONT_SIZE.x,
        Config::FONT_SIZE.y);

    term = LoadTerminal(Config::CANVAS_SIZE.x, Config::CANVAS_SIZE.y - 2, tmf);
    status = LoadTerminal(Config::WINDOW_SIZE.x, 3, tmf);
    overlay = LoadTerminal(Config::WINDOW_SIZE.x, Config::WINDOW_SIZE.y, tmf);

    cam = {
        {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f},
        {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f},
        0.0f, 1.0f
    };
    
    TerminalClear(term, {0xfa, p2, bg});

    palette = std::make_unique<Color[]>(status.width);
    for (int i=0; i<status.width; i++) {
        palette[i].a = 255;
        palette[i].r = (unsigned char)(rand() % 256);
        palette[i].g = (unsigned char)(rand() % 256);
        palette[i].b = (unsigned char)(rand() % 256);
    }

    lastHue = ColorToHSV(currentFg).x;
}

void CamNavigation::update() {

    if (redrawStatus) {
        redrawStatus = false;

        statusLeft.str("");
        statusLeft << "Text mode (" << (int)cursor.x << ", " << (int)cursor.y << ") " << statusLog;

        statusRight.str("");
        statusRight << "(" << (int)cursorGhost.x << ", " << (int)cursorGhost.y << ") " << cam.zoom << "x";

        BeginTextureMode(status.buffer);
        {
            TerminalDrawLine(status, 0, 0, status.width, 0, {'_', bg, BLANK});
            TerminalDrawLine(status, 0, 1, status.width, 1, {'_', bg, panel});
            TerminalDrawLine(status, 0, 2, status.width, 2, {' ', BLANK, panel});

            TerminalDrawXY(term, 2, 2, {'[', p2, panel});
            TerminalDrawXY(term, 3, 2, {'A', currentFg, currentBg});
            TerminalDrawXY(term, 4, 2, {']', p2, panel});

            TerminalDrawText(status, 6, 2, statusLeft.str(), p2, panel);
            TerminalDrawText(status, status.width-statusRight.str().length()-1, 2, statusRight.str(), p2, panel);
            for (int i=0; i<status.width; i++) {
                TerminalDrawXY(term, i, 1, {'_', bg, palette[i]});
            }
        }
        EndTextureMode();
    }

    BeginDrawing();
    {
        ClearBackground(panel);

        BeginMode2D(cam);
        {
            DrawTextureRec(
                term.buffer.texture,
                {0.0f, 0.0f, (float)term.buffer.texture.width, -1.0f*term.buffer.texture.height},
                {0.0f, 0.0f}, WHITE);

            DrawRectangle(
                cursorGhost.x*term.tmf.width,
                cursorGhost.y*term.tmf.height,
                term.tmf.width, term.tmf.height,
                ColorAlpha(ORANGE, 0.3));
        }
        EndMode2D();

        DrawTextureRec(
            status.buffer.texture,
            {0.0f, 0.0f, (float)status.buffer.texture.width, -1.0f*status.buffer.texture.height},
            {0.0f, (float)GetScreenHeight() - status.buffer.texture.height}, WHITE);

        // Vector3 hsv = ConvertRGBtoHSV((Vector3){ currentFg.r/255.0f, currentFg.g/255.0f, currentFg.b/255.0f });

        // hsv.x = lastHue;
        // Vector3 rgb = ConvertHSVtoRGB(hsv);

        // currentFg = {
        //     (unsigned char)roundf(rgb.x*255.0f),
        //     (unsigned char)roundf(rgb.y*255.0f),
        //     (unsigned char)roundf(rgb.z*255.0f),
        //     255
        // };

        // currentFg = GuiColorPicker(
        //     {0.0f, 0.0f, 400.0f, 400.0f},
        //     NULL, currentFg);

        // lastHue = GuiColorBarHue(
        //     {410.0f, 0.0f, 30.0f, 400.0f},
        //     NULL, lastHue);

        // if (CheckCollisionPointRec(GetMousePosition(), {410.0f, 0.0f, 30.0f, 400.0f})) {
        // }
        
        rlImGuiBegin();
        {

            ImGui::Begin(
                "Window", NULL, 
                ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoSavedSettings |
                ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::SetWindowPos({100.0f, GetScreenHeight() - ImGui::GetWindowHeight() - 30}, ImGuiCond_Always);

            static float color[4] = { 1.0f,1.0f,1.0f,1.0f };

            ImGui::ColorPicker4( 
                "Color", (float*)&color,
                ImGuiColorEditFlags_NoAlpha |
                ImGuiColorEditFlags_NoDragDrop |
                ImGuiColorEditFlags_NoLabel,
                NULL);

            ImGui::Button("Apply");
            ImGui::Button("Cancel");

            ImGui::End();


            // ImGui::Render();

        }
        rlImGuiEnd();
        
        if (enableFPS) DrawFPS(GetScreenWidth() - 100, 0);
    }
    EndDrawing();
    
    // ------------------------------
    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_LEFT_SHIFT)) enableFPS = !enableFPS;

    if (IsKeyDown(KEY_W)) cam.target.y -= (250.0f / cam.zoom) * dt;
    if (IsKeyDown(KEY_S)) cam.target.y += (250.0f / cam.zoom) * dt;
    if (IsKeyDown(KEY_A)) cam.target.x -= (250.0f / cam.zoom) * dt;
    if (IsKeyDown(KEY_D)) cam.target.x += (250.0f / cam.zoom) * dt;

    /// pan Drag
    if (IsKeyDown(KEY_SPACE)) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            dragStartPos = GetMousePosition();
            dragEndPos = GetMousePosition();
            SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (GetMouseX() != dragEndPos.x || GetMouseY() != dragEndPos.y) {
                dragEndPos = GetMousePosition();
                Vector2 delta = Vector2Subtract(dragStartPos, dragEndPos);
                dragStartPos = GetMousePosition();
                cam.target.x += delta.x * (1.0f / cam.zoom);
                cam.target.y += delta.y * (1.0f / cam.zoom);
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            SetMouseCursor(MOUSE_CURSOR_ARROW);
        }
    }

    if (lastScrollWheel != GetMouseWheelMove()) {
        lastScrollWheel = GetMouseWheelMove();
        cam.zoom += GetMouseWheelMove() * 0.5f;
        if (cam.zoom < 0.2f) cam.zoom = 0.5f;
        redrawStatus = true;
    }

    cursorGhost = GetScreenToWorld2D(GetMousePosition(), cam);
    cursorGhost = {
        std::floor(cursorGhost.x/term.tmf.width),
        std::floor(cursorGhost.y/term.tmf.height)
    };

    if (cursorGhost.x != cursorGhostTemp.x || cursorGhost.y != cursorGhostTemp.y) {
        redrawStatus = true;
        cursorGhostTemp = cursorGhost;
    }

    if (GetMouseY() > GetScreenHeight() - 2*term.tmf.height &&
        GetMouseY() < GetScreenHeight() - 1*term.tmf.height)
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            int colorIndex = std::floor(GetMouseX()/term.tmf.width);
            currentFg = palette[colorIndex];
            redrawStatus = true;
        }
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
        {
            int colorIndex = std::floor(GetMouseX()/term.tmf.width);
            currentBg = palette[colorIndex];
            redrawStatus = true;
        }
    }
}