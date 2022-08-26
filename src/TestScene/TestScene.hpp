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

class ConfigLoader : public melody::Scene {
public:
    ConfigLoader();
    void update() override;
};