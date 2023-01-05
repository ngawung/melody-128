#define NO_FONT_AWESOME

#include <string>
#include <iostream>
#include <fmt/core.h>
#include <raylib.h>
#include <raygui.h>
#include <raymath.h>
#include <imgui.h>
#include <rlImGui.h>
#include <nfd.h>

#include "TestScene/TestScene.hpp"
#include "utils/Config.hpp"
#include "utils/Terminal.hpp"

namespace
{
    TerminalFont tmf;

    bool isSelectCharacter;
    Vector2 CharacterCurrent;

    Rectangle tempCharacterPos;
    int menuBarHeight;
    int statusBarHeight;

    Texture2D text_tex;
    Texture2D line_tex;
    Texture2D rect_tex;
    Texture2D circle_tex;
    Texture2D fill_tex;
    Texture2D select_tex;
    Texture2D move_tex;

    Texture2D characterList;

    bool NewFile = false;

    void OpenFile() {
        NFD_Init();

        nfdchar_t *outPath;
        nfdresult_t result = NFD_SaveDialog(&outPath, NULL, 0, NULL, "project.ascii");

        if (result == NFD_OKAY)
        {
            puts("Success!");
            puts(outPath);
            NFD_FreePath(outPath);
        }
        else if (result == NFD_CANCEL)
        {
            puts("User pressed cancel.");
        }
        else 
        {
            printf("Error: %s\n", NFD_GetError());
        }

        NFD_Quit();
    }

    bool ButtonImageRect(const Texture *image, Rectangle sourceRect, float scale = 1.0f)
    {
        ImVec2 uv0;
        ImVec2 uv1;

        if (sourceRect.width < 0)
        {
            uv0.x = -((float)sourceRect.x / image->width);
            uv1.x = (uv0.x - (float)(fabs(sourceRect.width) / image->width));
        }
        else
        {
            uv0.x = (float)sourceRect.x / image->width;
            uv1.x = uv0.x + (float)(sourceRect.width / image->width);
        }

        if (sourceRect.height < 0)
        {
            uv0.y = -((float)sourceRect.y / image->height);
            uv1.y = (uv0.y - (float)(fabs(sourceRect.height) / image->height));
        }
        else
        {
            uv0.y = (float)sourceRect.y / image->height;
            uv1.y = uv0.y + (float)(sourceRect.height / image->height);
        }

        return ImGui::ImageButton(
            (ImTextureID)image, 
            {sourceRect.width, sourceRect.height},
            uv0, uv1);
    }

    void ShowMainMenuBar() 
    {
        if (ImGui::BeginMainMenuBar())
        {
            menuBarHeight = ImGui::GetWindowHeight();
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("New", "CTRL+N")) { NewFile = true; }
                if (ImGui::MenuItem("Open", "CTRL+O")) { OpenFile(); }
                ImGui::Separator();
                if (ImGui::MenuItem("Save", "CTRL+S")) {}
                if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) {}
                if (ImGui::BeginMenu("Export"))
                {
                    if (ImGui::MenuItem("to TXT")) {}
                    if (ImGui::MenuItem("to PNG")) {}
                    if (ImGui::MenuItem("to FILE")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Quit", "CTRL+X")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste As Selection")) {}
                if (ImGui::MenuItem("Paste As Document")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Select All", "CTRL+A")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Clear", "DELETE")) {}
                if (ImGui::MenuItem("Crop", "CTRL+R")) {}
                if (ImGui::MenuItem("Replace", "CTRL+H")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Modify"))
            {
                if (ImGui::MenuItem("Change Canvas Size")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("To Uppercase", "CTRL+ALT+U")) {}
                if (ImGui::MenuItem("To Lowercase", "CTRL+ALT+L")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Trim Whitespace")) {}
                if (ImGui::MenuItem("Trim By Selection")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Zoom In", "Page Up")) {}
                if (ImGui::MenuItem("Zoom Out", "Page Down")) {}
                ImGui::Separator();
                if (ImGui::BeginMenu("Font"))
                {
                    if (ImGui::MenuItem("Default")) {}
                    if (ImGui::MenuItem("Default 1")) {}
                    if (ImGui::MenuItem("Default 2")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Options")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About")) {}
                if (ImGui::MenuItem("Shortcut")) {}
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (NewFile) {
            ImGui::OpenPopup("NewFile");
            NewFile = false;
        }
            
        if (ImGui::BeginPopupModal("NewFile", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            static char str0[128] = "untitled.ascii";
            ImGui::InputText("Filename", str0, IM_ARRAYSIZE(str0));

            static int i0 = 100;
            ImGui::InputInt("Width", &i0);

            static int i1 = 40;
            ImGui::InputInt("Height", &i1);

            static float col1[3] = { 1.0f, 0.0f, 0.2f };
            ImGui::ColorEdit3("Background", col1);

            ImGui::Separator();
            if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }

    void ShowStatusBar()
    {
        ImGuiWindowFlags flag = 
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar;

        ImGuiColorEditFlags cflag =
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoLabel;

        ImGui::SetNextWindowPos({0.0f, (float)GetScreenHeight()}, ImGuiCond_Always, {0.0f, 1.0f});
        ImGui::SetNextWindowSize({(float)GetScreenWidth(), 0.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {5.0f, 2.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, {0.0f, 0.0f});

        static ImVec4 fg(255 , 0, 255, 255);
        static ImVec4 bg(255 , 0, 0, 255);
        static ImVec4 backup_color;

        static bool open_fg;
        static bool open_bg;

        ImGui::Begin("Status", NULL, flag);
        statusBarHeight = ImGui::GetWindowHeight();
        ImGui::PopStyleVar(2);

        ImGui::ColorEdit3("Foreground", (float*)&fg, cflag);
        ImGui::SameLine(0.0f, 2.0f);

        open_bg = ImGui::ColorButton("Background", bg, cflag);
        if (open_bg)
        {
             ImGui::OpenPopup("picker");
            backup_color = bg;
        }
        if (ImGui::BeginPopup("picker", ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Background");
            ImGui::Separator();
            ImGui::ColorPicker3("BackgroundPicker", (float*)&bg,
                ImGuiColorEditFlags_NoSmallPreview |
                ImGuiColorEditFlags_NoLabel | 
                ImGuiColorEditFlags_NoSidePreview);

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::TextUnformatted("Current");
            ImGui::ColorButton("current", bg, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            ImGui::TextUnformatted("Previous");
            if (ImGui::ColorButton("previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                bg = backup_color;
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::BeginGroup();
            ImGui::Text("Palette");
            for (int n = 0; n < 30; n++)
            {   
                if ((n % 5) != 0) ImGui::SameLine();
                ImGui::ColorButton("pallete", bg, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(20, 20));
            }
            ImGui::EndGroup();

            ImGui::EndPopup();
        }

        std::string mousePos = fmt::format("0x{0:X} ({0:d}) | ({1:d},{2:d})", 'Z', GetMouseX(), GetMouseY());

        ImGui::SameLine();
        rlImGuiImageRect(&tmf.texture, 8, 16, {
            (float)CharacterCurrent.x*tmf.width,
            (float)CharacterCurrent.y*tmf.height,
            (float)tmf.width,
            (float)tmf.height});

        ImGui::SameLine();
        ImGui::Text("Text Mode (%d, %d)", 3, 16);

        ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(mousePos.c_str()).x - 5);
        ImGui::TextUnformatted(mousePos.c_str());
        ImGui::End();
    }

    void ShowTool()
    {
        ImGuiWindowFlags flag = 
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoTitleBar;

        ImGui::SetNextWindowSize({0.0f, (float)GetScreenHeight() - menuBarHeight - statusBarHeight});
        ImGui::SetNextWindowPos({0.0f, (float)menuBarHeight});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {8.0f, 10.0f});

        ImGui::Begin("tool", NULL, flag);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&move_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&select_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&text_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&line_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&rect_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&circle_tex);
        // ImGui::Button(" ", {20.0f, 20.0f}); // rlImGuiImageButton(&fill_tex);
        ImGui::Separator();
        ImGui::End();

        ImGui::PopStyleVar();
    }

    void ShowCharacterSet()
    {
        ImGui::SetNextWindowPos({GetScreenWidth()*0.5f, GetScreenHeight()*0.5f}, ImGuiCond_Always, {0.5f, 0.5f});
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {16.0f, 10.0f});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {16.0f, 10.0f});

        if (ImGui::BeginPopup("CharacterSet", ImGuiWindowFlags_NoMove))
        {   
            isSelectCharacter = true;
            ImVec2 pos = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();

            tempCharacterPos = { pos.x, pos.y, size.x, size.y };
            rlImGuiImage(&characterList);

            // for (int n = 0; n < 256; n++)
            // {   
            //     if ((n % 16) != 0) ImGui::SameLine();
            //     // ImGui::Button(fmt::format("{}", n).c_str(), {10.0f, 20.0f});

            //     // ButtonImageRect(&tmf.texture, {
            //     //     (float)(n%16)*tmf.width,
            //     //     (float)(n/16)*tmf.height,
            //     //     (float)tmf.width,
            //     //     (float)tmf.height});
                    
            //     rlImGuiImageRect(&tmf.texture, 8, 16, {
            //         (float)(n%16)*tmf.width,
            //         (float)(n/16)*tmf.height,
            //         (float)tmf.width,
            //         (float)tmf.height});
            // }
            ImGui::EndPopup();
        } 
        else
        {
            isSelectCharacter = false;
        }
        
        ImGui::PopStyleVar(2);
    }
}

ImGuiLayout::ImGuiLayout()
{
    rlImGuiSetup(true);
    GuiLoadStyle(RESOURCE_PATH"default.rgs");

    tmf = LoadTerminalFont(
        (RESOURCE_PATH + Config::FONT_FILE).c_str(),
        Config::FONT_ROW,
        Config::FONT_SIZE.x,
        Config::FONT_SIZE.y);

    text_tex = LoadTexture(RESOURCE_PATH"text.png");
    line_tex = LoadTexture(RESOURCE_PATH"line.png");
    rect_tex = LoadTexture(RESOURCE_PATH"rectangle.png");
    circle_tex = LoadTexture(RESOURCE_PATH"circle.png");
    fill_tex = LoadTexture(RESOURCE_PATH"fill.png");
    select_tex = LoadTexture(RESOURCE_PATH"select.png");
    move_tex = LoadTexture(RESOURCE_PATH"move.png");

    RenderTexture temp = LoadRenderTexture(
        tmf.width*tmf.row + 16*(tmf.row-1),
        tmf.height*tmf.row + 10*(tmf.row-1));

    BeginTextureMode(temp);
    {
        float x, y;
        for (int i = 0; i < 256; i++)
        {
            x = (i%tmf.row);
            y = (i/tmf.row);

            DrawTextureRec(tmf.texture, {
                x*tmf.width, y*tmf.height,
                (float)tmf.width, (float)tmf.height
            }, {
                x*tmf.width + (16*x),
                y*tmf.height + (10*y)
            },WHITE);
        }
    }
    EndTextureMode();

    characterList = temp.texture;
    // UnloadRenderTexture(temp);
}

void ImGuiLayout::update()
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        rlImGuiBegin();
        {

            if (IsKeyPressed(KEY_TAB)) {
                ImGui::OpenPopup("CharacterSet");
            }

            // -----------------------------------

            static bool open = true;
            ImGui::ShowDemoWindow(&open);

            ShowMainMenuBar();
            ShowStatusBar();
            ShowCharacterSet();
            // ShowTool();
        }
        rlImGuiEnd();

        if (isSelectCharacter)
        {
            DrawRectangleLines(
                tempCharacterPos.x + 8 + CharacterCurrent.x*24, 
                tempCharacterPos.y + 5 + CharacterCurrent.y*26, 
                24, 26, ORANGE);

            DrawRectangleLines(
                tempCharacterPos.x + 16 + CharacterCurrent.x*24, 
                tempCharacterPos.y + 10 + CharacterCurrent.y*26, 
                8, 16, ColorAlpha(RAYWHITE, 0.2f));

            if (ImGui::IsMouseDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), tempCharacterPos))
            {
                CharacterCurrent = {
                    std::floor((GetMouseX()-tempCharacterPos.x-8) / 24),
                    std::floor((GetMouseY()-tempCharacterPos.y-5) / 26)
                };

                CharacterCurrent = Vector2Clamp(CharacterCurrent, {0.0f, 0.0f}, {15.0f, 15.0f});
            }

        }

        // DrawTextureRec(characterList, {
        //     0.0f, 0.0f,
        //     (float)characterList.width,
        //     -1.0f*characterList.height
        // }, {50.0f, 50.0f}, WHITE);
        
        GuiButton({GetScreenWidth() - 40.0f, 30, 30, 30}, "#68#"); // move
        GuiButton({GetScreenWidth() - 40.0f, 70, 30, 30}, "#109#"); // select
        GuiButton({GetScreenWidth() - 40.0f, 110, 30, 30}, "#24#"); // brush
        GuiButton({GetScreenWidth() - 40.0f, 150, 30, 30}, "#28#"); // ereaser
        GuiButton({GetScreenWidth() - 40.0f, 190, 30, 30}, "#31#"); // text
        GuiButton({GetScreenWidth() - 40.0f, 230, 30, 30}, "#192#"); // line
        GuiButton({GetScreenWidth() - 40.0f, 270, 30, 30}, "#99#"); // rect
        GuiButton({GetScreenWidth() - 40.0f, 310, 30, 30}, "#135#"); // circle
        GuiButton({GetScreenWidth() - 40.0f, 350, 30, 30}, "#29#"); // fill

        DrawFPS(GetScreenWidth() - 100, 0);
    }
    EndDrawing();
}

ImGuiLayout::~ImGuiLayout() {
    rlImGuiShutdown();
}