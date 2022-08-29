
#include <raygui.h>
#include <imgui.h>
#include <rlImGui.h>
#include <rlImGuiColors.h>
#include <nfd.h>
#include <fmt/core.h>

#include "utils/Config.hpp"
#include "scene/MainEditor.hpp"

#include <raymath.h>

namespace {
    inline Color Vec2Col(const ImVec4 &v)
    {
        return Color {
            (unsigned char)std::floor(v.x * 255.0f),
            (unsigned char)std::floor(v.y * 255.0f),
            (unsigned char)std::floor(v.z * 255.0f),
            (unsigned char)std::floor(v.w * 255.0f)};
    }
}

MainEditor::MainEditor()
{
    tmf = LoadTerminalFont(
        (RESOURCE_PATH + Config::FONT_FILE).c_str(),
        Config::FONT_ROW,
        Config::FONT_SIZE.x,
        Config::FONT_SIZE.y);

    term = LoadTerminal(Config::CANVAS_SIZE.x, Config::CANVAS_SIZE.y, tmf);
    termOverlay = LoadTerminal(Config::CANVAS_SIZE.x, Config::CANVAS_SIZE.y, tmf);

    cam = {
        {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f},
        {GetScreenWidth()/2.0f, GetScreenHeight()/2.0f},
        0.0f, 1.0f
    };

    TerminalClear(term, {0xfa, Vec2Col(p2), Vec2Col(bg)});
}

void MainEditor::update()
{
    logic();
    render();
}

void MainEditor::logic()
{
    /// pan Drag
    // if (IsKeyDown(KEY_SPACE))
    // {
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
        {
            dragStartPos = GetMousePosition();
            dragEndPos = GetMousePosition();
            SetMouseCursor(MOUSE_CURSOR_RESIZE_ALL);
        }

        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            if (GetMouseX() != dragEndPos.x || GetMouseY() != dragEndPos.y) {
                dragEndPos = GetMousePosition();
                Vector2 delta = Vector2Subtract(dragStartPos, dragEndPos);
                dragStartPos = GetMousePosition();
                cam.target.x += delta.x * (1.0f / cam.zoom);
                cam.target.y += delta.y * (1.0f / cam.zoom);
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
            SetMouseCursor(MOUSE_CURSOR_ARROW);

    // }

    if (lastScrollWheel != GetMouseWheelMove())
    {
        lastScrollWheel = GetMouseWheelMove();
        cam.zoom += GetMouseWheelMove() * 0.1f;
        if (cam.zoom < 0.1f) cam.zoom = 0.1f;
    }

    cursorGhost = GetScreenToWorld2D(GetMousePosition(), cam);
    cursorGhost = {
        std::floor(cursorGhost.x/term.tmf.width),
        std::floor(cursorGhost.y/term.tmf.height)
    };

    updateCursorNavigation();
    onCharacterInput(GetCharPressed());

    if (IsCursorHidden())
    {
        Vector2 temp = GetMousePosition();
        if (cursorIdlePos.x != temp.x && cursorIdlePos.y != temp.y)
            ShowCursor();
    }
}

void MainEditor::updateCursorNavigation() {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        cursor = cursorGhost;
        cursorIndex = cursor.x;
        colorAlpha = 1.0f;
    }

    static bool right_down = IsKeyDown(KEY_RIGHT);
    static bool left_down = IsKeyDown(KEY_LEFT);
    static bool up_down = IsKeyDown(KEY_UP);
    static bool down_down = IsKeyDown(KEY_DOWN);
    static bool back_down = IsKeyDown(KEY_BACKSPACE);

    // onDown repeated
    if (right_down || left_down || up_down || down_down || back_down)
    {
        pressCounter += GetFrameTime();
        if (pressCounter >= 0.3) pressDelay += GetFrameTime();

        if (pressDelay >= 0.05)
        {
            pressDelay = 0;
            if (right_down) MoveCursor(1, 0);
            if (left_down) MoveCursor(-1, 0);
            if (up_down) MoveCursor(0, -1);
            if (down_down) MoveCursor(0, 1);
            if (back_down)
            {
                TerminalDrawXY(term, cursor, {' ', Vec2Col(p1), Vec2Col(p2)});
                MoveCursor(-1, 0);
            }
        }
    }

    // onRelease
    if (IsKeyReleased(KEY_RIGHT) || 
        IsKeyReleased(KEY_LEFT) || 
        IsKeyReleased(KEY_UP) || 
        IsKeyReleased(KEY_DOWN) ||
        IsKeyReleased(KEY_BACKSPACE))
        pressCounter = pressDelay = 0;

    // onPressed
    if (IsKeyPressed(KEY_LEFT)) MoveCursor(-1, 0);
    if (IsKeyPressed(KEY_RIGHT)) MoveCursor(1, 0);
    if (IsKeyPressed(KEY_UP)) MoveCursor(0, -1);
    if (IsKeyPressed(KEY_DOWN)) MoveCursor(0, 1);
    if (IsKeyPressed(KEY_DELETE)) TerminalClear(term, {0xfa, Vec2Col(p1), Vec2Col(p2)});
    if (IsKeyPressed(KEY_ENTER))
    {
        MoveCursor(0, 1);
        cursor.x = cursorIndex;
        colorAlpha = 1.0f;
    }
    if (IsKeyPressed(KEY_BACKSPACE))
    {
        TerminalDrawXY(term, cursor, {' ', Vec2Col(p1), Vec2Col(p2)});
        MoveCursor(-1, 0);
    }
}

void MainEditor::onCharacterInput(int unicode) {
    if (unicode == 0) return;
    
    TerminalDrawXY(term, cursor, {(uint8_t)unicode, Vec2Col(p1), Vec2Col(p2)});
    MoveCursor(1, 0);
    colorAlpha = 1.0f;

    cursorIdlePos = GetMousePosition();
    HideCursor();
}

void MainEditor::render()
{
    BeginDrawing();
    {
        ClearBackground(Vec2Col(panel));

        BeginMode2D(cam);
        {
            DrawRenderTexture(term.buffer.texture, Vector2{0.0f, 0.0f});
            // DrawRenderTexture(termOverlay.buffer.texture);
            
            // boundary
            DrawRectangleLines(
                -1, -1, 
                term.buffer.texture.width+2, term.buffer.texture.height+2,
                ORANGE);

            // cursor ghost
            if (!IsCursorHidden())
            {
                DrawRectangle(
                    cursorGhost.x*term.tmf.width,
                    cursorGhost.y*term.tmf.height,
                    term.tmf.width, term.tmf.height,
                    ColorAlpha(ORANGE, 0.3));
            }

            // cursor
            DrawRectangleLines(
                (int)cursor.x*tmf.width, (int)cursor.y*tmf.height,
                tmf.width, tmf.height, ORANGE);

            // cursor index
            DrawLine(
                cursorIndex*tmf.width, 0,
                cursorIndex*tmf.width, term.height*tmf.height,
                ColorAlpha(ORANGE, colorAlpha));

            if (colorAlpha > 0) colorAlpha -= GetFrameTime()*2;
            if (colorAlpha < 0) colorAlpha = 0.0f;
        }
        EndMode2D();
    }

    handleImGui();

    if (IsKeyDown(KEY_LEFT_ALT))
    {
        HideCursor();
        GuiLabel(
            {GetMouseX()-2.0f, GetMouseY()-14.0f, 16.0f, 16.0f},
            "#27#");
    }
    if (IsKeyUp(KEY_LEFT_ALT)) ShowCursor();

    DrawFPS(GetScreenWidth() - 100, 0);
    EndDrawing();
}

void MainEditor::handleImGui()
{
    rlImGuiBegin();
    {
        if (IsKeyPressed(KEY_TAB))
            ImGui::OpenPopup("Character Selector");

        guiMainMenu();
        fileNew();
        
        guiStatus();

    }
    rlImGuiEnd();

    GuiButton({GetScreenWidth() - 40.0f, 30, 30, 30}, "#68#"); // move
    GuiButton({GetScreenWidth() - 40.0f, 70, 30, 30}, "#109#"); // select
    GuiButton({GetScreenWidth() - 40.0f, 110, 30, 30}, "#24#"); // brush
    GuiButton({GetScreenWidth() - 40.0f, 150, 30, 30}, "#28#"); // ereaser
    GuiButton({GetScreenWidth() - 40.0f, 190, 30, 30}, "#31#"); // text
    GuiButton({GetScreenWidth() - 40.0f, 230, 30, 30}, "#192#"); // line
    GuiButton({GetScreenWidth() - 40.0f, 270, 30, 30}, "#99#"); // rect
    GuiButton({GetScreenWidth() - 40.0f, 310, 30, 30}, "#135#"); // circle
    GuiButton({GetScreenWidth() - 40.0f, 350, 30, 30}, "#29#"); // fill
}

void MainEditor::guiMainMenu()
{
    static int _newfile = false;

    if (ImGui::BeginMainMenuBar())
    {
        _menuHeight = ImGui::GetWindowHeight();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New", "CTRL+N")) { _fileNew = true; }
            if (ImGui::MenuItem("Open", "CTRL+O")) { fileOpen(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "CTRL+S")) { fileSave(); }
            if (ImGui::MenuItem("Save As", "CTRL+SHIFT+S")) { fileSaveAs(); }
            if (ImGui::MenuItem("Export to Image", "CTRL+E")) { fileExport(); }
            if (ImGui::MenuItem("Quit", "CTRL+X")) { exitProgram(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) { editUndo(); }
            if (ImGui::MenuItem("Redo", "CTRL+Y")) { editRedo(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) { editCut(); }
            if (ImGui::MenuItem("Copy", "CTRL+C")) { editCopy(); }
            if (ImGui::MenuItem("Paste As Selection", "CTRL+SHIFT+V")) { editPaste(); }
            if (ImGui::MenuItem("Paste As Document", "CTRL+ALT+SHIFT+V")) { editPasteDoc(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Select All", "CTRL+A")) { editSelectAll(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Clear", "DELETE")) { editClear(); }
            if (ImGui::MenuItem("Crop", "CTRL+R")) { editCrop(); }
            if (ImGui::MenuItem("Replace", "CTRL+H")) { editReplace(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Modify"))
        {
            if (ImGui::MenuItem("Change Canvas Size")) { modCanvasResize(); }
            ImGui::Separator();
            if (ImGui::MenuItem("To Uppercase", "CTRL+ALT+U")) { modUppercase(); }
            if (ImGui::MenuItem("To Lowercase", "CTRL+ALT+L")) { modLowercase(); }
            ImGui::Separator();
            if (ImGui::MenuItem("Trim Whitespace")) { modTrim(); }
            if (ImGui::MenuItem("Trim By Selection")) { modTrimSelect(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Zoom In", "Page Up")) { cam.zoom += 0.5f; }
            if (ImGui::MenuItem("Zoom Out", "Page Down")) { cam.zoom -= 0.5f; }
            ImGui::Separator();
            if (ImGui::BeginMenu("Font"))
            {
                if (ImGui::MenuItem("Default")) {}
                if (ImGui::MenuItem("Default 1")) {}
                if (ImGui::MenuItem("Default 2")) {}
                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Options")) { viewOption(); }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About")) { helpAbout(); }
            if (ImGui::MenuItem("Shortcut")) { helpShortcut(); }
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void MainEditor::guiStatus()
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

    static ImVec4 backup_color;

    ImGui::Begin("Status", NULL, flag);
    _statusHeight = ImGui::GetWindowHeight();
    ImGui::PopStyleVar(2);

    ImGui::ColorEdit3("Foreground", (float*)&p1, cflag);
    ImGui::SameLine(0.0f, 2.0f);

    static bool open_fg;
    static bool open_bg;

    open_bg = ImGui::ColorButton("Background", p2, cflag);
    if (open_bg)
    {
         ImGui::OpenPopup("picker");
        backup_color = p2;
    }
    if (ImGui::BeginPopup("picker", ImGuiWindowFlags_NoMove))
    {
        ImGui::Text("Background");
        ImGui::Separator();
        ImGui::ColorPicker3("BackgroundPicker", (float*)&p2,
            ImGuiColorEditFlags_NoSmallPreview |
            ImGuiColorEditFlags_NoLabel | 
            ImGuiColorEditFlags_NoSidePreview);

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::TextUnformatted("Current");
        ImGui::ColorButton("current", p2, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
        ImGui::TextUnformatted("Previous");
        if (ImGui::ColorButton("previous", backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
            p2 = backup_color;
        ImGui::EndGroup();

        ImGui::SameLine();
        ImGui::BeginGroup();
        ImGui::Text("Palette");
        // for (int n = 0; n < 30; n++)
        // {   
        //     if ((n % 5) != 0) ImGui::SameLine();
        //     ImGui::ColorButton("pallete", bg, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(20, 20));
        // }
        ImGui::EndGroup();

        ImGui::EndPopup();
    }

    std::string mousePos = fmt::format("0x{0:X} ({0:d}) | ({1:d},{2:d})", term.chars[cursor.y*term.width + cursor.x].symbol, (int)cursorGhost.x, (int)cursorGhost.y);

    ImGui::SameLine();
    rlImGuiImageRect(&tmf.texture, 8, 16, {
        (float)charCurrent.x*tmf.width,
        (float)charCurrent.y*tmf.height,
        (float)tmf.width,
        (float)tmf.height});

    ImGui::SameLine();
    ImGui::Text("Text Mode (%d, %d)", (int)cursor.x, (int)cursor.y);

    ImGui::SameLine(ImGui::GetWindowWidth() - ImGui::CalcTextSize(mousePos.c_str()).x - 5);
    ImGui::TextUnformatted(mousePos.c_str());
    ImGui::End();
}

MainEditor::~MainEditor()
{
    UnloadTerminal(term);
    UnloadTerminal(termOverlay);
    UnloadTerminalFont(tmf);
}

// --------------

void MainEditor::DrawRenderTexture(Texture tex, Vector2 pos)
{
    DrawTextureRec(
        tex,
        {0.0f, 0.0f, (float)tex.width, -1.0f*tex.height},
        pos, WHITE);
}

void MainEditor::MoveCursor(int x, int y)
{
    cursor.x += x;
    cursor.y += y;

    if (cursor.x > term.width-1) {
        cursor.y++;
        cursor.x = cursorIndex;
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

///////////////////////////////////////////

void MainEditor::fileNew() {
    if (_fileNew)
    {
        ImGui::OpenPopup("File New");
        _fileNew = false;
    }

    if (ImGui::BeginPopupModal("File New", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::InputText("Filename", _filename, IM_ARRAYSIZE(_filename));

        static int i0 = 100;
        ImGui::InputInt("Width", &i0);

        static int i1 = 40;
        ImGui::InputInt("Height", &i1);

        ImGui::ColorEdit3("Background", (float*)&p2);

        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
        ImGui::EndPopup();
    }
}

void MainEditor::fileOpen()
{
    NFD_Init();

    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = { { "ASCII File", "ascii" }, { "Text File", "txt" } };
    nfdresult_t result = NFD_OpenDialog(&outPath, filterItem, 2, NULL);

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

void MainEditor::fileSave() {}
void MainEditor::fileSaveAs()
{
    NFD_Init();

    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[2] = { { "ASCII File", "ascii" }, { "Text File", "txt" } };
    nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 2, NULL, _filename);

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

void MainEditor::fileExport()
{
    NFD_Init();

    nfdchar_t *outPath;
    nfdfilteritem_t filterItem[1] = { { "Image File", "png" } };
    nfdresult_t result = NFD_SaveDialog(&outPath, filterItem, 1, NULL, _filename);

    if (result == NFD_OKAY)
    {
        Image img = LoadImageFromTexture(term.buffer.texture);
        ImageFlipVertical(&img);
        ExportImage(img, outPath);
        UnloadImage(img);

        NFD_FreePath(outPath);
    }
    else if (result == NFD_CANCEL)
    {
        
    }
    else 
    {
        printf("Error: %s\n", NFD_GetError());
    }

    NFD_Quit();
}

void MainEditor::exitProgram()
{
    melody::App::get_instance()->Running = false;
}

void MainEditor::editUndo() {}
void MainEditor::editRedo() {}
void MainEditor::editCut() {}
void MainEditor::editCopy() {}
void MainEditor::editPaste() {}
void MainEditor::editPasteDoc() {}
void MainEditor::editSelectAll() {}
void MainEditor::editClear() {}
void MainEditor::editCrop() {}
void MainEditor::editReplace() {}

void MainEditor::modCanvasResize() {}
void MainEditor::modUppercase() {}
void MainEditor::modLowercase() {}
void MainEditor::modTrim() {}
void MainEditor::modTrimSelect() {}

void MainEditor::viewOption() {}

void MainEditor::helpAbout() {}
void MainEditor::helpShortcut() {}