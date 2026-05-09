#pragma once
#include "raylib.h"

class Transition {
private:
    float timer;
    float duration;
    bool isActive;
    int levelNumber;
    int score;
    int stars;
    Font font;

public:
    Transition();
    ~Transition();

    void Start(int level, int playerScore, int playerStars);
    void Update();
    void Draw();
    bool IsFinished();
    void Reset();
};