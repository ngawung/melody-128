#pragma once

#include <raylib.h>
#include <imgui.h>
#include <melody.hpp>

#include "utils/Terminal.hpp"

class MainEditor : public melody::Scene {
public:
    MainEditor();
    ~MainEditor();
    void update() override;

private:
    TerminalFont tmf;
    Terminal term;
    Terminal termOverlay;

    Camera2D cam;
    float lastScrollWheel;
    Vector2 dragStartPos;
    Vector2 dragEndPos;

    ImVec4 bg    = {0.06f ,0.07f ,0.10f ,1.0f};
    ImVec4 panel = {0.12f ,0.14f ,0.16f ,1.0f};
    ImVec4 p1    = {0.68f ,0.68f ,0.58f ,1.0f};
    ImVec4 p2    = {0.12f ,0.14f ,0.16f ,1.0f};

    Vector2 cursor = {0.0f, 0.0f};
    Vector2 cursorGhost = {0.0f, 0.0f};
    Vector2 cursorIdlePos = {0.0f, 0.0f};
    int cursorIndex = 0;

    Vector2 charCurrent;
    unsigned char charCurrentId;

    float pressCounter = 0;
    float pressDelay = 0;
    float colorAlpha = 0.0f;

    void logic();
    void render();

    void updateCursorNavigation();
    void onCharacterInput(int unicode);
    void handleImGui();

    void DrawRenderTexture(Texture tex, Vector2 pos);
    void MoveCursor(int x, int y);

    void guiMainMenu();
    void guiStatus();

    int _menuHeight;
    int _statusHeight;
    char _filename[128] = "untitled.ascii";
    bool _fileNew = false;

    void fileNew();
    void fileOpen();
    void fileSave();
    void fileSaveAs();
    void fileExport();
    void exitProgram();

    void editUndo();
    void editRedo();
    void editCut();
    void editCopy();
    void editPaste();
    void editPasteDoc();
    void editSelectAll();
    void editClear();
    void editCrop();
    void editReplace();

    void modCanvasResize();
    void modUppercase();
    void modLowercase();
    void modTrim();
    void modTrimSelect();
    
    void viewOption();
    
    void helpAbout();
    void helpShortcut();
};