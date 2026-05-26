#pragma once
#include "raylib.h"

class DebugMenu {
private:
    bool infiniteStars;
    bool noHammerCooldown;
    int selectedOption;
    Font font;

public:
    DebugMenu();
    ~DebugMenu();

    void Update();
    void Draw();

    bool IsInfiniteStars() const { return infiniteStars; }
    bool IsNoHammerCooldown() const { return noHammerCooldown; }
    bool ShouldContinue() const { return selectedOption == 0; }
};