#pragma once

#include <melody.hpp>

class TerminalBasic : public melody::Scene {
public:
    TerminalBasic();
    void update() override;

private:
    bool right_press, left_press, up_press, down_press, back_press;
    
    void moveCursor(int x, int y);
    void cursorInput(int keycode);
    void characterInput(int unicode);
};

class UILayout : public melody::Scene {
public:
    UILayout();
    void update() override;
};

class CamNavigation : public melody::Scene {
public:
    CamNavigation();
    void update() override;
};

class ImGuiLayout : public melody::Scene {
public:
    ImGuiLayout();
    ~ImGuiLayout();
    void update() override;
};

class EpicGLFW : public melody::Scene {
public:
    EpicGLFW();
    ~EpicGLFW();
    void update() override;
};
