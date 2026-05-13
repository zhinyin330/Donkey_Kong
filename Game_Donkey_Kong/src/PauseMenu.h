#pragma once
#include "raylib.h"

enum class PauseOption {
    CONTINUE,
    MAIN_MENU
};

class PauseMenu {
private:
    PauseOption selectedOption;
    Texture2D frameTexture;
    Font font;
    bool isActive;

public:
    PauseMenu();
    ~PauseMenu();

    void Show();
    void Hide();
    bool IsActive() const { return isActive; }

    void Update();
    void Draw();
    PauseOption GetSelectedOption() const { return selectedOption; }
};