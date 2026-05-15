#pragma once
#include "raylib.h"
#include <string>

class GameOver {
private:
    Font font;
    bool isActive;
    float timer;
    bool showNameInput;      // Primero Game Over, luego pedir nombre
    std::string playerName;
    int maxChars;
    bool finished;

public:
    GameOver();
    ~GameOver();

    void Show();
    void Hide();
    bool IsActive() const { return isActive; }
    bool IsFinished() const { return finished; }
    std::string GetPlayerName() const { return playerName; }

    void Update();
    void Draw();
};